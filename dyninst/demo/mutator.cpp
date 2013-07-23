#include <dyninst/BPatch.h>
#include <dyninst/BPatch_point.h>
#include <dyninst/BPatch_function.h>
#include <vector>

int main (int argc, const char* argv[]) {
    BPatch bpatch;
    BPatch_process *proc = bpatch.processCreate(argv[2], argv + 2);
    bpatch.setTrampRecursive(true);
    bpatch.setSaveFPR(false);

    BPatch_object *ipa = proc->loadLibrary(argv[1]);
    BPatch_image *image = proc->getImage();

    std::vector<BPatch_function *> foo_fns, ipa_fns;
    image->findFunction("foo", foo_fns);
    image->findFunction("tptest", ipa_fns);

    std::vector<BPatch_snippet*> args;
    BPatch_funcCallExpr call_ipa(*ipa_fns[0], args);
    proc->insertSnippet(call_ipa, (foo_fns[0]->findPoint(BPatch_entry))[0]);

    proc->continueExecution();
    while (!proc->isTerminated()) {
        bpatch.waitForStatusChange();
    }

    return 0;
}
