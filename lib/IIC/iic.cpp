#include <iic.h>

void IICinit()
{
    Wire.begin(SDA_PIN, SCL_PIN);
}


