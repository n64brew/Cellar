#include <string>
#include "markdown.h"


/*==============================
    md4c_funcptr_handlestr
    A helper function for appending strings during html 
    generation from markdown
    @param  The input markdown text
    @param  The size of the input markdown text
    @param  A pointer to the outputted content
==============================*/

void md4c_funcptr_handlestr(const MD_CHAR* input, MD_SIZE inputsize, void* output)
{
    std::string* str = static_cast<std::string*>(output);
    str->append(input, inputsize);
}