#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

#ifndef NULL
#ifndef __cplusplus
#define NULL (void*)0
#else
#define NULL 0
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void unityOutputStart();
void unityOutputChar(char);
void unityOutputFlush();
void unityOutputComplete();

#ifndef UNITY_OUTPUT_START
#define UNITY_OUTPUT_START()    unityOutputStart()
#endif 
#ifndef UNITY_OUTPUT_CHAR
#define UNITY_OUTPUT_CHAR(c)    unityOutputChar(c)
#endif
#ifndef UNITY_OUTPUT_FLUSH
#define UNITY_OUTPUT_FLUSH()    unityOutputFlush()
#endif
#ifndef UNITY_OUTPUT_COMPLETE
#define UNITY_OUTPUT_COMPLETE() unityOutputComplete()
#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* UNITY_CONFIG_H */
