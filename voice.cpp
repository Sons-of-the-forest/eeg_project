#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <Python.h>
#include <string.h>
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){
    Py_Initialize();
    string returnWord="xin chào";
    PyObject* voiceModule=PyImport_ImportModule("pyttsx3");
    PyObject* engine=PyObject_GetAttrString(voiceModule, "engine");
    string script="import pyttsx3\nengine=pyttsx3.init()\nvoices=engine.getProperty('voices')\nengine.setProperty('voice',voices[70].id)\nengine.say('"+returnWord+"')\nengine.runAndWait()";
    cout<< script.c_str() <<endl;
    PyRun_SimpleString(script.c_str());
    Py_Finalize();
}