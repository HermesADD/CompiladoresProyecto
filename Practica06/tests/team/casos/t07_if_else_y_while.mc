int n = 3;
bool activo = true;
while (n > 0 && activo) {
    if (n == 1) { activo = false; }
    else { n = n - 1; }
}
