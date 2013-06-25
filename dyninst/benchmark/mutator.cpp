#include <BPatch.h>
#include <BPatch_object.h>
#include <BPatch_point.h>
#include <BPatch_function.h>
#include <vector>

int main (int argc, const char* argv[]) {
    BPatch bpatch;

    // argv[2] is muttee's file name, will be muttee's argv[0]
    BPatch_process *proc = bpatch.processCreate(argv[2], argv + 2);

    // Options to tune performance
    //bpatch.setTrampRecursive(true);
    //bpatch.setSaveFPR(false);
    //bpatch.setInstrStackFrames(false);

    BPatch_object *ipa = proc->loadLibrary(argv[1]);
    BPatch_image *image = proc->getImage();

    std::vector<BPatch_function *> tracepoints, probes;
    image->findFunction("do_stuff", tracepoints);
    BPatch_function *tracepoint = tracepoints[0];
    image->findFunction("tpbench", probes);
    BPatch_function *probe = probes[0];

    BPatch_variableExpr* var = image->findVariable("v");

    int* val;
    val = (int *) var->getBaseAddr();

    std::vector<BPatch_snippet*> args;
    BPatch_constExpr val_expr(val);
    args.push_back(&val_expr);
    BPatch_funcCallExpr call_probe(*probe, args);
    proc->insertSnippet(call_probe, (tracepoint->findPoint(BPatch_exit))[0]);

    proc->continueExecution();
    while (!proc->isTerminated()) {
        bpatch.waitForStatusChange();
    }

    return 0;
}