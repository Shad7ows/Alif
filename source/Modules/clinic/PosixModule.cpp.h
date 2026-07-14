






































































// 11419
#define OS_CPU_COUNT_METHODDEF    \
    {"عدد_المعالجات", (AlifCPPFunction)os_cpuCount, METHOD_NOARGS},

static AlifObject* os_cpuCountImpl(AlifObject*);

static AlifObject* os_cpuCount(AlifObject* _module,
	AlifObject* ALIF_UNUSED(ignored)) { // 11425
	return os_cpuCountImpl(_module);
}
