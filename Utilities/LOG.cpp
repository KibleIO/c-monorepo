#include "LOG.h"

void Log (string message)
{
	#ifdef LOG_CONSOLE
	LOG_CONSOLE << message << endl;
	#endif // LOG_CONSOLE

	#ifdef LOG_FILE
	ofstream file_stream;
	file_stream.open(LOG_FILE, ios::out | ios::app);
	if (file_stream.is_open())
	{
		file_stream << message << "\n";
		file_stream.close();
	}
	#ifdef LOG_CONSOLE
	else
	{
		LOG_CONSOLE << "WARNING: @LOG::Log(message: '" << message << "') Failed to open log file '" << LOG_FILE << "'." << endl;
	}
	#endif // LOG_CONSOLE
	#endif // LOG_FILE
}

void Error     (string message)
{
	Log("ERROR: " + message);

	exit (EXIT_FAILURE);
}
void Warning   (string message)
{
	Log("WARNING: " + message);
}
void Notice    (string message)
{
	Log("NOTICE: " + message);
}
void Debugging (string message)
{
	Log("DEBUGGING: " + message);
}
