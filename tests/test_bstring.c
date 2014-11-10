#include "core/minunit.h"
#include <assert.h>
#include "bstrlib/bstrlib.h"

char *test_to_learn()
{
    bstring s = bfromcstr("HOLA");
    mu_assert(bstricmp(s, bfromcstr("hola")) == 0, "No es igual a cero");

    return NULL;
}

/* TODO: Testear estas funciones

bfromcstr Create a bstring from a C style constant.
blk2bstr Same but give the length of the buffer.
bstrcpy      Copy a bstring.
bassign      Set one bstring to another.
bassigncstr  Set a bstring to a C string's contents.
bassignblk   Set a bstring to a C string but give the length.
bdestroy     Destroy a bstring.
bconcat      Concatenate one bstring onto another.
bstricmp     Compare two bstrings returning the same result as strcmp.
biseq        Tests if two bstrings are equal.
binstr       Tells if one bstring is in another.
bfindreplace Find one bstring in another then replace it with a third.
bsplit       How to split a bstring into a bstrList.
bformat      Doing a format string, super handy.
blength      Getting the length of a bstring.
bdata        Getting the data from a bstring.
bchar        Getting a char from a bstring.

Your test should try out all of these operations, and a few more that you find interesting from the header file. Make sure to run the test under valgrind to make sure you use the memory correctly.

*/

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_to_learn);

    return NULL;
}

RUN_TESTS(all_tests)

