Custom Unix Shell in C

Description:
A Unix shell written in C that executes terminal commands.

Key Features:
- Process Management: Utilizes fork(), execvp(), and waitpid() to create and manage child processes for executing standard system commands (e.g., ls, cat, grep).
- Built-in Commands: Custom implementation of essential shell commands, including cd for directory navigation using and exit.
- Command Parsing: Efficient string manipulation and tokenization (strtok) to handle user inputs with multiple arguments and variable whitespace.

How to Compile and Run
1. Open a Unix/Linux terminal and navigate to the project directory.
2. Compile the source code using gcc
3. Run the compiled executable
4. You will see the custom shell prompt ($). You can now type commands, use arguments, or chain commands using a pipe (e.g., ls -l | wc -l).
