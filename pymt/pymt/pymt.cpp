// pymt.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include "Python.h"
#include <string>

using namespace std;

extern "C" __declspec(dllexport) void LoadScript(wchar_t* path, wchar_t* name);
extern "C" __declspec(dllexport) void Finalize();
extern "C" __declspec(dllexport) void NewArg(int i);
extern "C" __declspec(dllexport) void CallFunc(wchar_t* name);
extern "C" __declspec(dllexport) long GetRezLong();
extern "C" __declspec(dllexport) double GetRezDouble();
extern "C" __declspec(dllexport) wchar_t* GetRezString();
extern "C" __declspec(dllexport) void GetRezArrayLong(int* arr);
extern "C" __declspec(dllexport) void GetRezArrayDouble(double* arr);
extern "C" __declspec(dllexport) void SetArgLong(int i, long data);
extern "C" __declspec(dllexport) void SetArgDouble(int i, double data);
extern "C" __declspec(dllexport) void SetArgString(int i, wchar_t* data);
extern "C" __declspec(dllexport) void SetArgArrayLong(int ind, int size, int* data);
extern "C" __declspec(dllexport) void SetArgArrayDouble(int ind, int size, double* data);

PyObject *pModule;
PyObject *pFunc;
PyObject *pArgs;
PyObject *pValue;
PyObject *global_dict;

void LoadScript(wchar_t* path, wchar_t* name)
{
	wstring w(name);
	string st(w.begin(), w.end());
	const char * s = st.c_str();
	wstring w1(path);
	string st1(w1.begin(), w1.end());
	string st2 = st1 + st;
	const char * s1 = st2.c_str();
	const char * s3 = st1.c_str();
	Py_Initialize();
	PyObject* sysPath = PySys_GetObject((char*)"path");
	PyObject* curDir = PyUnicode_FromString((char*)s3);
	PyList_Append(sysPath, curDir);
	Py_DECREF(curDir);
	Py_DECREF(sysPath);
	FILE *file = _Py_fopen(s1, "r+");
	if (file != NULL) {
		PyRun_SimpleFile(file, s);
	}
	PyObject *pName;
	pName = PyUnicode_FromString("__main__");
	pModule = PyImport_Import(pName);
	global_dict = PyModule_GetDict(pModule);
}


void Finalize()
{
	Py_DECREF(global_dict);
	Py_DECREF(pModule);
	Py_Finalize();
}

void NewArg(int i)
{
	pArgs = PyTuple_New(i);
}

void CallFunc(wchar_t* name)
{
	wstring w(name);
	string st(w.begin(), w.end());
	const char * s = st.c_str();
	pFunc = PyDict_GetItemString(global_dict, s);
	if (pFunc && PyCallable_Check(pFunc))
	{
		pValue = PyObject_CallObject(pFunc, pArgs);
		Py_DECREF(pArgs);
	}
}

long GetRezLong()
{
	long rez = 0;
	if (pValue != NULL)
	{
		rez = PyLong_AsLong(pValue);
		Py_DECREF(pValue);
	}
	return rez;
}

double GetRezDouble()
{
	double rez = 0.0;
	if (pValue != NULL)
	{
		rez = PyFloat_AsDouble(pValue);
		Py_DECREF(pValue);
	}
	return rez;
}

wchar_t* GetRezString()
{
	wchar_t* rez = (wchar_t*)L"";
	if (pValue != NULL)
	{
		rez = PyUnicode_AsWideCharString(pValue,NULL);
		Py_DECREF(pValue);
	}
	return rez;
}

void GetRezArrayLong(int* arr)
{
	if (pValue != NULL)
	{
		Py_ssize_t n;
		n = PyList_Size(pValue);
		for (int i = 0; i < n; i++)
		{
			PyObject *value = PyList_GetItem(pValue, i);
			arr[i]= (int) PyLong_AsLong(value);
		}
		Py_DECREF(pValue);
	}
}

void GetRezArrayDouble(double* arr)
{
	if (pValue != NULL)
	{
		Py_ssize_t n;
		n = PyList_Size(pValue);
		for (int i = 0; i < n; i++)
		{
			PyObject *value = PyList_GetItem(pValue, i);
			arr[i] = PyFloat_AsDouble(value);
		}
		Py_DECREF(pValue);
	}
}

void SetArgLong(int i, long data)
{
	PyTuple_SetItem(pArgs, i, PyLong_FromLong(data));
}

void SetArgDouble(int i, double data)
{
	PyTuple_SetItem(pArgs, i, PyFloat_FromDouble(data));
}

void SetArgString(int i, wchar_t* data)
{
	PyTuple_SetItem(pArgs, i, PyUnicode_FromWideChar(data,NULL));
}

void SetArgArrayLong(int ind, int size, int* data)
{
	PyObject* pList = PyList_New(size);
	for (int i = 0; i < size; i++)
		PyList_SetItem(pList, i, PyLong_FromLong(data[i]));
	PyTuple_SetItem(pArgs, ind, pList);
}

void SetArgArrayDouble(int ind, int size, double* data)
{
	PyObject* pList = PyList_New(size);
	for (int i = 0; i < size; i++)
		PyList_SetItem(pList, i, PyFloat_FromDouble(data[i]));
	PyTuple_SetItem(pArgs, ind, pList);
}
