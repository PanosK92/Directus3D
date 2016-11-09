/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ====================
#include "Log.h"
#include "../Events/EventHandler.h"
#include <sstream> 
#include <fstream>
#include "ILogger.h"
#include "../FileSystem/FileSystem.h"
//===============================

//= NAMESPACES ================
using namespace std;
using namespace Directus::Math;
//=============================

weak_ptr<ILogger> Log::m_logger;
ofstream Log::m_fout;
string Log::m_logFileName = "log.txt";
bool Log::m_firstLog = true;

void Log::Initialize()
{

}

void Log::Release()
{

}

void Log::SetLogger(weak_ptr<ILogger> logger)
{
	m_logger = logger;
}

//= LOGGING ==========================================================================
void Log::Write(const string& text, LogType type) // all functions resolve to that one
{
	string prefix = "";

	if (type == Info)
		prefix = "Info:";

	if (type == Warning)
		prefix = "Warning:";

	if (type == Error)
		prefix = "Error:";

	string finalText = prefix + " " + text;

	auto logger = m_logger.lock();
	logger ? logger->Log(finalText, type) : WriteAsText(finalText);
	// if a logger is available use it, if not output a text file
}

void Log::WriteAsText(const string& text)
{
	// Delete the previous log file (if it exists)
	if (m_firstLog)
	{
		FileSystem::DeleteFile_(m_logFileName);
		m_firstLog = false;
	}

	// Open/Create a log file to write the error message to.
	m_fout.open(m_logFileName, ofstream::out | ofstream::app);

	// Write out the error message.
	m_fout << text << endl;

	// Close the file.
	m_fout.close();
}

void Log::Write(const char* text, LogType type)
{
	string str = text;
	Write(str, type);
}

void Log::Write(const Vector3& vector, LogType type)
{
	string x = "X: " + to_string(vector.x);
	string y = "Y: " + to_string(vector.y);
	string z = "Z: " + to_string(vector.z);

	Write(x + ", " + y + ", " + z, type);
}

void Log::Write(const Quaternion& quaternion, LogType type)
{
	string x = "X: " + to_string(quaternion.x);
	string y = "Y: " + to_string(quaternion.y);
	string z = "Z: " + to_string(quaternion.z);
	string w = "W: " + to_string(quaternion.w);

	Write(x + ", " + y + ", " + z + ", " + w, type);
}

void Log::Write(float value, LogType type)
{
	Write(to_string(value), type);
}

void Log::Write(int value, LogType type)
{
	Write(to_string(value), type);
}

void Log::Write(unsigned int value, LogType type)
{
	Write(int(value), type);
}

void Log::Write(bool value, LogType type)
{
	if (value)
		Write("True", type);
	else
		Write("False", type);
}

void Log::Write(size_t value, LogType type)
{
	Write(int(value), type);
}
//=================================================================================

//= HELPER FUNCTIONS ==============================================================
string Log::WCHARPToString(WCHAR* text)
{
	wstring ws(text);
	string str(ws.begin(), ws.end());

	return str;
}
