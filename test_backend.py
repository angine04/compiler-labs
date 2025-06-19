#!/usr/bin/env python3

"""
Backend testing script for minic compiler
Tests ARM assembly generation and execution via qemu
Compares results with reference IRCompiler
"""

import os
import sys
import subprocess
import time
from pathlib import Path
from dataclasses import dataclass
from typing import Optional, List, Tuple
import argparse

@dataclass
class TestResult:
    test_name: str
    frontend: str
    ir_success: bool
    asm_success: bool
    binary_success: bool
    runtime_success: bool
    output_matches: bool
    exit_code_matches: bool
    error_message: Optional[str] = None
    total_time: float = 0.0

class BackendTester:
    def __init__(self, project_root: str, timeout: int = 30, frontends: List[str] = None):
        self.project_root = Path(project_root)
        self.timeout = timeout
        self.frontends = frontends or ["flex_bison", "antlr4", "recursive_descent"]
        
        # Tool paths
        self.minic = self.project_root / "build" / "minic"
        self.ir_compiler = self.project_root / "tools" / "IRCompiler" / "Linux-aarch64" / "Ubuntu-22.04" / "IRCompiler"
        self.test_dir = self.project_root / "tests" / "commonclasstestcases" / "function"
        
        # Create output directories
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        self.output_dir = self.project_root / "test_results" / f"backend_{timestamp}"
        for subdir in ["logs", "ir", "asm", "binary", "output", "ref_output"]:
            (self.output_dir / subdir).mkdir(parents=True, exist_ok=True)
        
        print(f"Backend test output: {self.output_dir}")
        self._check_tools()
    
    def _check_tools(self):
        """Check required tools availability"""
        tools = ["arm-linux-gnueabihf-gcc", "qemu-arm"]
        for tool in tools:
            try:
                subprocess.run([tool, "--version"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
            except:
                print(f"ERROR: {tool} not found. Install with: sudo apt-get install gcc-arm-linux-gnueabihf qemu-user")
                sys.exit(1)
    
    def run_cmd(self, cmd: List[str], timeout: int = None, input_file: str = None, 
                stdout_file: str = None, stderr_file: str = None) -> Tuple[int, float]:
        """Run command with timeout"""
        start_time = time.time()
        timeout = timeout or self.timeout
        
        stdout_handle = open(stdout_file, 'w') if stdout_file else subprocess.PIPE
        stderr_handle = open(stderr_file, 'w') if stderr_file else subprocess.PIPE
        
        try:
            if input_file:
                with open(input_file, 'r') as f:
                    process = subprocess.Popen(
                        cmd,
                        stdin=f,
                        stdout=stdout_handle,
                        stderr=stderr_handle,
                        cwd=self.project_root
                    )
            else:
                process = subprocess.Popen(
                    cmd,
                    stdout=stdout_handle,
                    stderr=stderr_handle,
                    cwd=self.project_root
                )
            
            try:
                exit_code = process.wait(timeout=timeout)
                return exit_code, time.time() - start_time
            except subprocess.TimeoutExpired:
                process.kill()
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    pass
                return -999, time.time() - start_time
                
        finally:
            if stdout_file and stdout_handle != subprocess.PIPE:
                stdout_handle.close()
            if stderr_file and stderr_handle != subprocess.PIPE:
                stderr_handle.close()
    
    def compile_to_ir(self, test_file: Path, frontend: str) -> Tuple[bool, str]:
        """Compile source to IR"""
        test_name = test_file.stem
        ir_file = self.output_dir / "ir" / f"{test_name}_{frontend}.ir"
        log_file = self.output_dir / "logs" / f"{test_name}_{frontend}_ir.log"
        
        frontend_flags = {"antlr4": ["-A"], "recursive_descent": ["-D"]}.get(frontend, [])
        cmd = [str(self.minic), "-S", "-I"] + frontend_flags + [str(test_file), "-o", str(ir_file)]
        
        exit_code, _ = self.run_cmd(cmd, stdout_file=str(log_file), stderr_file=str(log_file))
        
        if exit_code == 0 and ir_file.exists() and ir_file.stat().st_size > 0:
            return True, str(ir_file)
        return False, ""
    
    def compile_to_asm(self, test_file: Path, frontend: str) -> Tuple[bool, str]:
        """Compile source to ARM assembly"""
        test_name = test_file.stem
        asm_file = self.output_dir / "asm" / f"{test_name}_{frontend}.s"
        log_file = self.output_dir / "logs" / f"{test_name}_{frontend}_asm.log"
        
        frontend_flags = {"antlr4": ["-A"], "recursive_descent": ["-D"]}.get(frontend, [])
        cmd = [str(self.minic), "-S"] + frontend_flags + [str(test_file), "-o", str(asm_file)]
        
        exit_code, _ = self.run_cmd(cmd, stdout_file=str(log_file), stderr_file=str(log_file))
        
        if exit_code == 0 and asm_file.exists() and asm_file.stat().st_size > 0:
            return True, str(asm_file)
        return False, ""
    
    def compile_asm_to_binary(self, asm_file: str, test_name: str, frontend: str) -> Tuple[bool, str]:
        """Compile assembly to binary"""
        binary_file = self.output_dir / "binary" / f"{test_name}_{frontend}"
        log_file = self.output_dir / "logs" / f"{test_name}_{frontend}_gcc.log"
        
        # Add std.c for library function support
        std_c_file = self.test_dir / "std.c"
        cmd = ["arm-linux-gnueabihf-gcc", "-static", "-o", str(binary_file), asm_file, str(std_c_file)]
        exit_code, _ = self.run_cmd(cmd, stdout_file=str(log_file), stderr_file=str(log_file))
        
        if exit_code == 0 and Path(binary_file).exists():
            return True, str(binary_file)
        return False, ""
    
    def run_binary(self, binary_file: str, test_name: str, frontend: str) -> Tuple[bool, str, int]:
        """Run binary on qemu"""
        output_file = self.output_dir / "output" / f"{test_name}_{frontend}.out"
        log_file = self.output_dir / "logs" / f"{test_name}_{frontend}_run.log"
        input_file = self.test_dir / f"{test_name}.in"
        
        cmd = ["qemu-arm", binary_file]
        
        if input_file.exists():
            exit_code, _ = self.run_cmd(cmd, input_file=str(input_file), 
                                      stdout_file=str(output_file), stderr_file=str(log_file))
        else:
            exit_code, _ = self.run_cmd(cmd, stdout_file=str(output_file), stderr_file=str(log_file))
        
        output = ""
        if output_file.exists():
            try:
                output = output_file.read_text().strip()
            except:
                pass
        
        # Check for runtime errors (segfaults, etc.)
        # Only negative exit codes indicate signal-based termination in subprocess
        # Exit codes 128-255 can be normal program return values (e.g., return -1 becomes 255)
        if exit_code == -999:
            runtime_success = False  # Timeout
        elif exit_code < 0:
            runtime_success = False  # Signal-based termination (negative signal number from subprocess)
        else:
            runtime_success = True  # Normal exit codes (0-255 are all valid)
        
        return runtime_success, output, exit_code
    
    def run_reference(self, ir_file: str, test_name: str) -> Tuple[bool, str, int]:
        """Run reference IRCompiler"""
        output_file = self.output_dir / "ref_output" / f"{test_name}_ref.out"
        log_file = self.output_dir / "logs" / f"{test_name}_ref.log"
        input_file = self.test_dir / f"{test_name}.in"
        
        cmd = [str(self.ir_compiler), "-R", ir_file]
        
        if input_file.exists():
            exit_code, _ = self.run_cmd(cmd, input_file=str(input_file),
                                      stdout_file=str(output_file), stderr_file=str(log_file))
        else:
            exit_code, _ = self.run_cmd(cmd, stdout_file=str(output_file), stderr_file=str(log_file))
        
        output = ""
        if output_file.exists():
            try:
                output = output_file.read_text().strip()
            except:
                pass
        
        # Check for runtime errors (segfaults, etc.)
        if exit_code == -999:
            runtime_success = False  # Timeout
        elif exit_code < 0:
            runtime_success = False  # Signal-based termination (negative signal number from subprocess)
        else:
            runtime_success = True  # Normal exit codes (0-255 are all valid)
        
        return runtime_success, output, exit_code
    
    def test_single_file(self, test_file: Path) -> List[TestResult]:
        """Test single file with all frontends"""
        test_name = test_file.stem
        results = []
        
        print(f"Testing {test_name}...")
        
        # Get reference output using flex_bison
        reference_output, reference_exit = None, None
        if "flex_bison" in self.frontends:
            ir_success, ir_file = self.compile_to_ir(test_file, "flex_bison")
            if ir_success:
                ref_success, ref_output, ref_exit = self.run_reference(ir_file, test_name)
                if ref_success:
                    reference_output, reference_exit = ref_output, ref_exit
        
        # Test each frontend
        for frontend in self.frontends:
            start_time = time.time()
            result = TestResult(test_name, frontend, False, False, False, False, False, False)
            
            print(f"  {frontend}: ", end="", flush=True)
            
            # Step 1: Compile to IR
            ir_success, ir_file = self.compile_to_ir(test_file, frontend)
            result.ir_success = ir_success
            if not ir_success:
                result.error_message = "IR compilation failed"
                print("IR FAILED")
                result.total_time = time.time() - start_time
                results.append(result)
                continue
            
            # Step 2: Compile to ASM
            asm_success, asm_file = self.compile_to_asm(test_file, frontend)
            result.asm_success = asm_success
            if not asm_success:
                result.error_message = "ASM generation failed"
                print("ASM FAILED")
                result.total_time = time.time() - start_time
                results.append(result)
                continue
            
            # Step 3: Compile to binary
            binary_success, binary_file = self.compile_asm_to_binary(asm_file, test_name, frontend)
            result.binary_success = binary_success
            if not binary_success:
                result.error_message = "Binary compilation failed"
                print("BINARY FAILED")
                result.total_time = time.time() - start_time
                results.append(result)
                continue
            
            # Step 4: Run binary
            runtime_success, output, exit_code = self.run_binary(binary_file, test_name, frontend)
            result.runtime_success = runtime_success
            if not runtime_success:
                if exit_code == -999:
                    result.error_message = "Runtime timeout"
                    print("RUNTIME TIMEOUT")
                elif exit_code == -11 or exit_code == 139:  # SIGSEGV
                    result.error_message = "Segmentation fault (SIGSEGV)"
                    print("RUNTIME FAILED (SIGSEGV)")
                elif exit_code == -6 or exit_code == 134:  # SIGABRT
                    result.error_message = "Aborted (SIGABRT)"
                    print("RUNTIME FAILED (SIGABRT)")
                elif exit_code < 0:
                    signal_num = -exit_code
                    result.error_message = f"Runtime error (signal {signal_num})"
                    print(f"RUNTIME FAILED (SIG{signal_num})")
                else:
                    result.error_message = f"Runtime error (exit code {exit_code})"
                    print(f"RUNTIME FAILED ({exit_code})")
                result.total_time = time.time() - start_time
                results.append(result)
                continue
            
            # Step 5: Compare with reference
            if reference_output is not None and reference_exit is not None:
                result.output_matches = (output == reference_output)
                result.exit_code_matches = (exit_code == reference_exit)
                if result.output_matches and result.exit_code_matches:
                    print("OK")
                else:
                    print("OUTPUT MISMATCH")
                    result.error_message = "Output or exit code mismatch"
            else:
                result.output_matches = True
                result.exit_code_matches = True
                print("OK (no reference)")
            
            result.total_time = time.time() - start_time
            results.append(result)
        
        return results
    
    def run_tests(self, pattern: str = "*.c", max_tests: int = None) -> List[TestResult]:
        """Run tests on matching files"""
        test_files = list(self.test_dir.glob(pattern))
        
        # Filter and sort test files
        filtered_files = []
        for test_file in test_files:
            if test_file.name.startswith(('std.', 'minicrun', 'readme')):
                continue
            try:
                # Extract test number for sorting
                test_number = int(test_file.name.split('_')[0])
                if test_number <= 143:  # Basic tests only for now
                    filtered_files.append((test_number, test_file))
            except:
                continue
        
        test_files = [f for _, f in sorted(filtered_files)]
        if max_tests:
            test_files = test_files[:max_tests]
        
        print(f"Found {len(test_files)} test files")
        
        all_results = []
        for i, test_file in enumerate(test_files, 1):
            print(f"[{i}/{len(test_files)}] ", end="")
            try:
                results = self.test_single_file(test_file)
                all_results.extend(results)
            except KeyboardInterrupt:
                print(f"Interrupted at {test_file.name}")
                break
            except Exception as e:
                print(f"ERROR: {e}")
        
        return all_results
    
    def print_summary(self, results: List[TestResult]):
        """Print test summary"""
        print("\n" + "="*70)
        print("BACKEND TEST SUMMARY")
        print("="*70)
        
        for frontend in self.frontends:
            frontend_results = [r for r in results if r.frontend == frontend]
            if not frontend_results:
                continue
            
            total = len(frontend_results)
            ir_ok = sum(1 for r in frontend_results if r.ir_success)
            asm_ok = sum(1 for r in frontend_results if r.asm_success)
            binary_ok = sum(1 for r in frontend_results if r.binary_success)
            runtime_ok = sum(1 for r in frontend_results if r.runtime_success)
            output_ok = sum(1 for r in frontend_results if r.output_matches)
            exit_ok = sum(1 for r in frontend_results if r.exit_code_matches)
            full_ok = sum(1 for r in frontend_results if all([
                r.ir_success, r.asm_success, r.binary_success, 
                r.runtime_success, r.output_matches, r.exit_code_matches
            ]))
            
            print(f"\n{frontend.upper()}:")
            print(f"  IR generation:      {ir_ok:3d}/{total} ({100*ir_ok/total:5.1f}%)")
            print(f"  ASM generation:     {asm_ok:3d}/{total} ({100*asm_ok/total:5.1f}%)")
            print(f"  Binary compilation: {binary_ok:3d}/{total} ({100*binary_ok/total:5.1f}%)")
            print(f"  Runtime success:    {runtime_ok:3d}/{total} ({100*runtime_ok/total:5.1f}%)")
            print(f"  Output matches:     {output_ok:3d}/{total} ({100*output_ok/total:5.1f}%)")
            print(f"  Exit code matches:  {exit_ok:3d}/{total} ({100*exit_ok/total:5.1f}%)")
            print(f"  FULL SUCCESS:       {full_ok:3d}/{total} ({100*full_ok/total:5.1f}%)")
        
        # Show failures
        failures = [r for r in results if not all([
            r.ir_success, r.asm_success, r.binary_success, 
            r.runtime_success, r.output_matches, r.exit_code_matches
        ])]
        
        if failures:
            print(f"\nFAILURES ({len(failures)}):")
            for result in failures[:20]:  # Show first 20 failures
                print(f"  {result.test_name} ({result.frontend}): {result.error_message}")
            if len(failures) > 20:
                print(f"  ... and {len(failures) - 20} more")
        else:
            print("\nNo failures!")
        
        print(f"\nResults saved in: {self.output_dir}")

def main():
    parser = argparse.ArgumentParser(description="Test minic backend (ARM assembly generation)")
    parser.add_argument("--frontends", nargs="+", 
                       choices=["flex_bison", "antlr4", "recursive_descent"],
                       default=["flex_bison", "antlr4", "recursive_descent"],
                       help="Frontends to test")
    parser.add_argument("--max-tests", type=int, help="Max number of tests")
    parser.add_argument("--pattern", default="*.c", help="Test file pattern")
    parser.add_argument("--timeout", type=int, default=30, help="Timeout per operation")
    parser.add_argument("--test-files", nargs="+", help="Specific test files")
    
    args = parser.parse_args()
    
    # Verify project structure
    project_root = Path(__file__).parent.absolute()
    
    minic = project_root / "build" / "minic"
    if not minic.exists():
        print("ERROR: minic not found. Run 'cd build && make' first.")
        sys.exit(1)
    
    ir_compiler = project_root / "tools" / "IRCompiler" / "Linux-aarch64" / "Ubuntu-22.04" / "IRCompiler"
    if not ir_compiler.exists():
        print(f"ERROR: IRCompiler not found at {ir_compiler}")
        sys.exit(1)
    
    test_dir = project_root / "tests" / "commonclasstestcases" / "function"
    if not test_dir.exists():
        print(f"ERROR: Test directory not found: {test_dir}")
        sys.exit(1)
    
    # Run tests
    tester = BackendTester(str(project_root), args.timeout, args.frontends)
    
    try:
        if args.test_files:
            all_results = []
            for test_file_name in args.test_files:
                test_file = test_dir / test_file_name
                if test_file.exists():
                    results = tester.test_single_file(test_file)
                    all_results.extend(results)
                else:
                    print(f"WARNING: {test_file_name} not found")
        else:
            all_results = tester.run_tests(args.pattern, args.max_tests)
        
        tester.print_summary(all_results)
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        sys.exit(1)

if __name__ == "__main__":
    main() 