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
    PyObject* voiceModule=PyImport_ImportModule("pyttsx3");
    PyObject* requests=PyImport_ImportModule("requests");
    PyObject* pd=PyImport_ImportModule("pandas");
    PyObject* engine=PyObject_GetAttrString(voiceModule, "engine");
    string script="import pyttsx3\nimport requests\nimport pandas as pd\n"
    "df = pd.read_csv('dataRaw/datnuoc_add_add/datnuoc_add_add27.csv')\n"
    "list_object = df[' Brainwave Value'].to_list()\n"
    "response = requests.post('http://9966-34-124-161-212.ngrok.io/api/predict', json={'list': list_object})\n"
    "print(response.text)\n"
    "engine=pyttsx3.init()\n"
    "voices=engine.getProperty('voices')\n"
    "engine.setProperty('voice',voices[70].id)\n"
    "engine.say(response.text)\n"
    "engine.runAndWait()";
    PyRun_SimpleString(script.c_str());
    Py_Finalize();
}