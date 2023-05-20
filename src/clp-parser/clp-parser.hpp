#pragma once

#include <iostream>

#include <rang.hpp>

#include <fstream>
#include <sstream>

#include <vector>
#include <string>
#include <algorithm>
#include <queue>


/** \brief Commandline parameters parser.
 * \details This namespace provides access to Command and Parser classes
 * these classes process a string of tokens received by the program at startup
 */
namespace clpp {
    
    // Code for CLI parser.
    inline std::string global_Path; ///< Path to main image
    inline bool cerr_disabled = true; ///< Permission to display
    inline std::vector<std::string> allowed_Format = {"ppm", "png"}; ///< Allowed file formats

    /** \brief Help function
     * \details This function show help message with allowed command and rules for writing them.
     */
    void help();

    /** \brief Version show function
     * \details This function show version of GGPEG.
     */
    void version();
    
    /** \brief Enumeration specifying the type of command.
     * \details Used to set the type of command in the Сommand class.
    */
    enum class CommandType
    {
        crop = 0, 
        rotate = 1, 
        resize = 2,
        negative = 3, // n
        insert = 4,
        convert_to = 5,
        reflect_x = 6, // x
        reflect_y = 7, // y
        version = 8, // v
        help = 9 // h
    };

    /** \brief A class for storing information about the command that can be executed on an image. 
     * 
     * \details This class contains two private fields describing the type and parameters of the
     * command. Subsequently, the parser forms a queue of instances of this class.
     * It also has functions to retrieve and display the command and its parameters.
     * 
    */
    class Command
    {
        public:

            /** \brief Constructs a Command object from a string command.
             * 
             * The constructor takes in a string command and parses it to extract the command type
             * and its parameters. Command object stores the command type and its
             * parameters.
             * 
             * \param command command consisting of a string 
            */
            Command(std::string& command);

            /** \brief Destructs for Command class. 
            */
            ~Command();
            /** \brief Displays the command parameters
             * 
             * This function displays command parameters
             * 
            */
            void show_param();

            /** \brief Displays the command type
             * 
             * This function display command type
             * 
            */
            void show_command();
        
            /** \brief Retrieves parameter values
             * 
             *  This function retrieves parameter values of the command
             * 
             * \return A vector of strings representing the parameters of the command.
            */
            std::vector<std::string> get_param();
        
            /** \brief  Retrieves command type
             * 
             * This function retrieves type of the command
             * 
             * \return One of the types of commands presented in CommandType
            */
            CommandType get_command();
            
            
        private:
            CommandType _command; ///< Type of command
            std::vector<std::string> _param; ///< Vector of strings from parameter values

            /** \brief Parses the parameters for a crop command.
             * 
             * This function parses the parameters for a crop command and stores them in _param.
             * 
             * \param second_part A string representing the parameters for the crop command
            */
            void _parser_param_crop(const std::string& second_part);

            /** \brief Parses the parameters for a rotate command.
             * 
             * This function parses the parameters for a rotate command and stores them in _param.
             * 
             * \param second_part A string representing the parameters for the rotate command
            */
            void _parser_param_rotate(const std::string& second_part);

            /** \brief Parses the parameters for a resize command.
             * 
             * This function parses the parameters for a resize command and stores them in _param.
             * 
             * \param second_part A string representing the parameters for the resize command
            */
            void _parser_param_resize(const std::string& second_part);

            /** \brief Parses the parameters for a insert command.
             * 
             * This function parses the parameters for a insert command and stores them in _param.
             * 
             * \param second_part A string representing the parameters for the insert command
            */
            void _parser_param_insert(const std::string& second_part);

            /** \brief Parses the parameters for a convert_to command.
             * 
             * This function parses the parameters for a convert_to command and stores them in
             * _param.
             * 
             * \param second_part A string representing the parameters for the convert_to command
            */
            void _parser_param_convert_to(const std::string& second_part);
            
            /** \brief Throws an exception if the path to a new image is invalid.
             * 
             * This function throws an exception if the path to a new image is invalid.
             * 
             * \param path Path to new image
            */
            void _exceptions_for_new_image(const std::string &path);

            /** Display error message and stop program
             * 
             * This function displays error message and stops program
             * 
            */
            void _error_param();

    };


    /** \brief A class for parsing tokens and generating correct commands for them.
     * 
     * 
     * \details Token - A set of strings taken from the command line. A token is not synonymous 
     *         with a command, because unlike a command, a token may contain abbreviated formats, 
     *         incorrect command names, or incorrectly passed parameters.
     * Command - A completely correct task for the program.
     * 
     * This class contains five private fields describing the string of tokens, string of commands, 
     * queue of commands and two constant field for contain correct short commands and long commands.
     * 
     * It also has functions to retrieve and display all fields.
     * 
    */
    class Parser
    {
    public:
        /** \brief Constructs a queue from Command objects from a string command.
         *
         * \details The constructor takes in a string of tokens and parses it to extract the
         * command. The constructor accepts a string of tokens, then looks for errors in it,
         * if errors are found, a message about this is displayed and the program stops,
         * if there are no errors,  the _parse _to_string method is called. In the called method,
         * tokens that are a set of short and long commands are reduced to a single format of long
         * commands. After that, the _parse_to_queue method is called, in which commands
         * that are strings are converted into a command class and a queue is created.
         *
         * \param token string with token from commandline
         * \param display permission to display
        */
        Parser(const std::vector<std::string> &tokens, const bool display);

        /** \brief Destructor for Parser class.
        */
        ~Parser();

        /** \brief Displays tokens
         *
         * \details This function displays tokens.
         *
        */
        void show_tokens();

        /** \brief Displays string of commands.
         *
         * \details This function displays command in string format.
         *
        */
        void show_line_of_command();

        /** \brief Displays queue of commands.
         *
         * \details This function displays queue of commands.
         *
        */
        void show_queue_of_command();

        /** \brief Displays set of correct short command.
         *
         * \details This displays set of correct short command.
         *
        */
        void show_correct_short_command();

        /** \brief Displays set of correct long command.
         *
         * \details This displays set of correct long command.
         *
        */
        void show_correct_long_command();

        /** \brief Retrieves tokens.
         *
         *  \details This function retrieves tokens.
         *  \return _tokens Vector of token Strings
        */
        std::vector<std::string> get_tokens();

        /** \brief Retrieves command string.
         *
         *  \details This function retrieves command string.
         *  \return _line_of_command Vector of command Strings
        */
        std::vector<std::string> get_line_of_command();

        /** \brief Retrieves command queue.
         *
         *  \details This function retrieves command queue.
         *  \return _queue_of_command Queue of commands
        */
        std::queue<Command> get_queue_of_command();

    private:

        const std::string _correct_short_command{"nxyvh"}; ///< String with correct short command
        const std::vector<std::string> _correct_long_commands = {"crop", "rotate", "resize",
                                                     "negative", "insert", "convert_to",
                                                     "reflect_x", "reflect_y", "version",
                                                     "help"}; ///< Vector with correct long command
        std::vector<std::string> _tokens; ///< Vector with tokens
        std::vector<std::string> _line_of_command; ///< Vector with commands
        std::queue<Command> _queue_of_command; ///< Queue with commands

        /** \brief Parse tokens to string.
         *
         * \details This function take vector of token string and convert them to vector
         * with command strings. After conversion, we save the received lines from the commands
         * to _line_of_command.
         *
        */
        void _parse_to_string();

        /** \brief Convert short command to long.
         *
         * \details This function convert short command to long.
         * \return one of the commands
        */
        std::string _convert_to_long(const char sh);

        /** \brief Parse string commands to queue
         *
         * \details This function takes a vector from the string commands obtained as a result of
         * executing the _parse_to_string function and converts each string command into an instance
         * of the Command class, after which it adds this element to the _queue_of_command queue.
         *
        */
        void _parse_to_queue();

        /** \brief Check for token with combination of short command.
         *
         * \details This function checks tokens that are a combination of short commands.
         * The check is performed by searching in the string of acceptable abbreviations
         * _correct_short_command of the abbreviation used in the token, if this abbreviation
         * is not found in the resolvable ones, then the _exceptions_for_short_command function is
         * called
         *
         * \param combination_short_command Сombination of short commands.
        */
        void _check_combination_short_command(const std::string &combination_short_command);

        /** \brief Check for token with long command
         *
         * \details This function checks tokens containing long commands. In this case, the check is
         * performed by searching for a token in the vector of allowed values
         * _correct_long_commands, if the token is not found among the allowed ones,
         * the _exceptions_for_incorrect_format function is called.
         *
         * \param long_command Long command.
        */
        void _check_long_command(const std::string &long_command);

        /** \brief Exception for non-existent images or unsupported formats.
         *
         * \details This function displays an error message and stops the program
         *
         * \param path Path to image
        */
        void _exceptions_for_image(const std::string &path);

        /** \brief Exception for incorrect short command.
         *
         * \details This function displays an error message and stops the program
         *
         * \param token Incorrect token
        */
        void _exceptions_for_short_command(const char token);

        /** \brief Exception for incorrect format of command.
         *
         * \details This function displays an error message and stops the program
         *
         * \param token Incorrect token
        */
        void _exceptions_for_incorrect_format(const std::string &token);
    };
}
