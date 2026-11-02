int global = 2;
{
    bool flag = true;
    {
        int local = global + 1;
        if (flag) { print(local); }
    }
}
print(global);
