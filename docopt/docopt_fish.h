#include <string>
#include <vector>
#include <map>

namespace docopt_fish
{
    /* Our docopt classes are parameterized by a string, expected to be either std::wstring or std::string */
    enum {
        flags_default = 0,
        
        /* If set, specifies that we should generate arguments even for unspecified values. These will have a count of 0. */
        flag_generate_empty_args = 1 << 0,
        
        /* If set, long options may be resolved using unambiguous prefixes. */
        flag_resolve_unambiguous_prefixes = 1 << 1,
    };
    typedef unsigned int parse_flags_t;
    
    /* Status of an argument */
    enum argument_status_t {
        status_invalid, // the argument doesn't work
        status_valid, // the argument works fine
        status_valid_prefix // the argument is a prefix of something that may work
    };
    
    /* Class representing a range of a string */
    struct range_t {
        size_t start;
        size_t length;
        range_t() : start(0), length(0) {}
        range_t(size_t s, size_t l) : start(s), length(l) {}
        
        /* Returns start + length, dying on overflow */
        size_t end() const;
        
        /* Returns length == 0 */
        bool empty() const;
        
        /* Equality and inequality */
        bool operator==(const range_t &rhs) const;
        bool operator!=(const range_t &rhs) const;

        /* Merges a range into this range. After merging, the receiver is the smallest range containing every index that is in either range. Empty ranges are discarded. */
        void merge(const range_t &rhs);
    };
    
    /* Represents an error. */
    template<typename string_t>
    struct error_t {
        /* Location of the token where the error occurred */
        size_t location;
        
        /* Text of the error */
        string_t text;
    };
    
    /* A processed docopt file is called an argument parser. */
    template<typename string_t> class docopt_impl;
    
    template<typename string_t>
    class argument_parser_t {
        /* Guts */
        string_t src;
        docopt_impl<string_t> *impl;
        
        public:
        static argument_parser_t *create(const string_t &doc, std::vector<error_t<string_t> > *out_errors);
        
        /* Given a list of arguments, this returns a corresponding parallel array validating the arguments */
        std::vector<argument_status_t> validate_arguments(const std::vector<string_t> &argv, parse_flags_t flags);
        
        ~argument_parser_t();
    };
    
    
    /* Represents an argument in the result */
    template<typename string_t>
    struct base_argument_t {
        /* How many times the argument appeared. This is typically 1 but may be greater than 1 for repeated arguments ("-v -v"), or 0 for missing arguments. */
        unsigned int count;
        
        /* The values specified in the argument. If this argument is a flag (like -d), this will be empty. If the argument has a single value, this will have a single value. If the argument has a default value, and no value was found in argv, the default will be contained in here (and count will be 0) */
        std::vector<string_t> values;
        
        /* Helper function to return a single value */
        const string_t &value() const {
            return values.at(0);
        }
        
        /* Constructor */
        base_argument_t() : count(0) {}
    };
    
    /* Concrete types */
    typedef base_argument_t<std::string> argument_t;
    typedef base_argument_t<std::wstring> wargument_t;
    
    /* Result of a docopt operation is a map from keys to values */
    std::map<std::string, argument_t> docopt_parse(const std::string &doc, const std::vector<std::string> &argv, parse_flags_t flags, std::vector<size_t> *out_unused_arguments = NULL);

    /* Wide variant */
    std::map<std::wstring, wargument_t> docopt_wparse(const std::wstring &doc, const std::vector<std::wstring> &argv, parse_flags_t flags, std::vector<size_t> *out_unused_arguments = NULL);
    
};