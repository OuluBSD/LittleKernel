#include "Runtime.h"

NAMESPACE_UPP

CodeProcess::CodeProcess() {}

int CodeProcess::GetPhaseCount() const { return PHASE_COUNT; }
int CodeProcess::GetBatchCount(int phase) const { return 1; }
int CodeProcess::GetSubBatchCount(int phase, int batch) const { return 1; };

void CodeProcess::DoPhase() {}

CodeProcess& CodeProcess::Get(String id)
{
	static ArrayMap<String, CodeProcess> proc;
	return proc.GetAdd(id);
}

END_UPP_NAMESPACE
