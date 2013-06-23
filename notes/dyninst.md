### Performance ###
* Overhead
  * Save General Purpose Registers
  * Save Floating Point Registers
    * To turn it off: BPatch::setSaveFPR(false)
    * Might not safe to turn it off if we want to capture floating variables.
    * Dyninst will attempt to analyse if saving floating point registers is required.
  * Generate A Stack Frame
    * To turn it off: BPatch::setInstrStackFrames(false)
    * Default is false.
  * Calculate Thread Index
  * Test and Set Trampoline Guard
  * Execute User Instrumentation
  * Unset Trampoline Guard
  * Clean Stack Frame
  * Restore Floating Point Registers
  * Restore General Purpose Registers

### Debug ###
~~~~~~~~~
DYNINST_DEBUG_MALWARE: Enabling DyninstAPI malware debug
DYNINST_DEBUG_TRAP: Enabling DyninstAPI debugging using traps
DYNINST_DEBUG_SPRINGBOARD: Enabling DyninstAPI springboard debug
DYNINST_DEBUG_STARTUP: Enabling DyninstAPI startup debug
DYNINST_DEBUG_PARSING: Enabling DyninstAPI parsing debug
DYNINST_DEBUG_PARSE: Enabling DyninstAPI parsing debug
DYNINST_DEBUG_DYNPC, DYNINST_DEBUG_FORKEXEC, DYNINST_DEBUG_INFRPC, DYNINST_DEBUG_SIGNAL, DYNINST_DEBUG_INFERIORRPC, DYNINST_DEBUG_THREAD, DYNINST_DEBUG_MAILBOX, DYNINST_DEBUG_DBI : Enabling DyninstAPI process control debug
DYNINST_DEBUG_STACKWALK: Enabling DyninstAPI stack walking debug
DYNINST_DEBUG_INST: Enabling DyninstAPI inst debug
DYNINST_DEBUG_RELOC: Enabling DyninstAPI relocation debug
DYNINST_DEBUG_RELOCATION: Enabling DyninstAPI relocation debug
DYNINST_DEBUG_SENSITIVITY: Enabling DyninstAPI sensitivity debug
DYNINST_DEBUG_DYN_UNW: Enabling DyninstAPI dynamic unwind debug
DYNINST_DEBUG_MUTEX: Enabling DyninstAPI mutex debug
DYNINST_DEBUG_RTLIB: Enabling DyninstAPI RTlib debug
DYNINST_DEBUG_CATCHUP: Enabling DyninstAPI catchup debug
DYNINST_DEBUG_BPATCH: Enabling DyninstAPI bpatch debug
DYNINST_DEBUG_REGALLOC: Enabling DyninstAPI register allocation debug
DYNINST_DEBUG_AST: Enabling DyninstAPI ast debug
DYNINST_DEBUG_WRITE: Enabling DyninstAPI process write debugging
DYNINST_DEBUG_INFMALLOC, DYNINST_DEBUG_INFERIORMALLOC: Enabling DyninstAPI inferior malloc debugging
DYNINST_DEBUG_CRASH: Enable DyninstAPI crash debugging
DYNINST_DEBUG_DISASS: Enabling DyninstAPI instrumentation disassembly debugging
~~~~~~~~~
Reference: http://git.dyninst.org/?p=dyninst.git;a=blob;f=dyninstAPI/src/debug.C
