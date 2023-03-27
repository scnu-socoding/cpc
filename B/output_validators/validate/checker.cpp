#include "testlib.h"

int main(int argc, char *argv[])
{
    registerTestlibCmd(argc, argv);
    int n = inf.readInt(1, 1000);
    while (n--)
    {
        int l = inf.readInt(-100, 100);
        int r = inf.readInt(-100, 100);
        int user = ouf.readInt(l, r);
    }
    quitf(_ok, "The answer is correct.");
}