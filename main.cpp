#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

string mySubstr(const string &str, int start, int length = -1)
{
  string result = "";
  if (length == -1)
  {
    for (int i = start; i < str.length(); i++)
    {
      result += str[i];
    }
  }
  else
  {
    for (int i = start; i < start + length && i < str.length(); i++)
    {
      result += str[i];
    }
  }
  return result;
}

bool isExecutable(string filepath)
{
  return access(filepath.c_str(), X_OK) == 0;
}

vector<string> parseCommand(const string &input)
{
  vector<string> args;
  string current = "";

  for (int i = 0; i < input.length(); i++)
  {
    if (input[i] == ' ')
    {
      if (current.length() > 0)
      {
        args.push_back(current);
        current = "";
      }
    }
    else
    {
      current += input[i];
    }
  }

  if (current.length() > 0)
  {
    args.push_back(current);
  }

  return args;
}

string findInPath(const string &command)
{
  char *path_env = getenv("PATH");
  if (path_env == nullptr)
    return "";

  string path = path_env;
  string currentDir = "";

  for (int i = 0; i <= path.length(); i++)
  {
    if (i == path.length() || path[i] == ':')
    {
      if (currentDir.length() > 0)
      {
        string fullPath = currentDir + "/" + command;
        if (access(fullPath.c_str(), X_OK) == 0)
        {
          return fullPath;
        }
      }
      currentDir = "";
    }
    else
    {
      currentDir += path[i];
    }
  }

  return "";
}

void executeCommand(const vector<string> &args)
{
  string command = args[0];
  string fullPath = findInPath(command);

  if (fullPath.empty())
  {
    cout << command << ": command not found\n";
    return;
  }

  // Convert to char* array for execve
  vector<char *> argv;
  for (int i = 0; i < args.size(); i++)
  {
    argv.push_back(const_cast<char *>(args[i].c_str()));
  }
  argv.push_back(nullptr);

  pid_t pid = fork();

  if (pid == 0)
  {
    // Child process
    execve(fullPath.c_str(), argv.data(), nullptr);
    // If execve returns, it failed
    cerr << "Failed to execute " << command << "\n";
    exit(1);
  }
  else if (pid > 0)
  {
    // Parent process - wait for child
    int status;
    waitpid(pid, &status, 0);
  }
  else
  {
    cerr << "Fork failed\n";
  }
}

int main()
{
  cout << unitbuf;
  cerr << unitbuf;

  while (true)
  {
    cout << "$ ";
    string input;
    getline(cin, input);

    if (mySubstr(input, 0, 4) == "exit")
    {
      return 0;
    }
    else if (mySubstr(input, 0, 5) == "echo ")
    {
      cout << mySubstr(input, 5) << "\n";
    }
    else if (mySubstr(input, 0, 5) == "type ")
    {
      string command = mySubstr(input, 5);

      if (command == "echo" || command == "exit" || command == "type")
      {
        cout << command << " is a shell builtin\n";
      }
      else
      {
        char *path_env = getenv("PATH");

        if (path_env == nullptr)
        {
          cout << command << ": not found\n";
        }
        else
        {
          string path = path_env;
          string currentDir = "";
          bool found = false;

          for (int i = 0; i <= path.length(); i++)
          {
            if (i == path.length() || path[i] == ':' || path[i] == ';')
            {
              if (currentDir.length() > 0)
              {
                string fullPath = currentDir + "/" + command;
                if (isExecutable(fullPath))
                {
                  cout << command << " is " << fullPath << "\n";
                  found = true;
                  break;
                }

                fullPath = currentDir + "/" + command + ".exe";
                if (isExecutable(fullPath))
                {
                  cout << command << " is " << fullPath << "\n";
                  found = true;
                  break;
                }
              }

              currentDir = "";
            }
            else
            {
              currentDir += path[i];
            }
          }

          if (!found)
          {
            cout << command << ": not found\n";
          }
        }
      }
    }
    else
    {
      // Try to execute external command
      vector<string> args = parseCommand(input);
      if (!args.empty())
      {
        executeCommand(args);
      }
    }
  }
}