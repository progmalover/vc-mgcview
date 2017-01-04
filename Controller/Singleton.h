#ifndef  _SINGLETON_
#define  _SINGLETON_

#define DECLARE_SINGLETON(T)\
public:\
 static T*Instance();

#define IMPLEMENT_SINGLETON(T)\
 T  G_##T##Instance;\
 T* T##::Instance()\
 {\
	return  &(G_##T##Instance) ;\
 }


#define DECLARE_DYNC_SINGLETON(T)\
public:\
 static  T*  p_##T##Instance;\
 static T*Instance();\
 static void FreeInstance();

#define IMPLEMENT_DYNC_SINGLETON(T)\
 T* T##:: p_##T##Instance;\
 T* T##:: Instance()\
 {\
	if(p_##T##Instance == NULL)\
		p_##T##Instance = new T;\
	return  (p_##T##Instance) ;\
 }\
 void T##:: FreeInstance()\
 {\
   if(p_##T##Instance)\
		delete p_##T##Instance;\
  p_##T##Instance = NULL;\
 }


#endif