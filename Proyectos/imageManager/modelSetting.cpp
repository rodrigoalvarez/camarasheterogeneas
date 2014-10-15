#include "modelSetting.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;

Model_SET::Model_SET() {

}

bool Model_SET::MemoryLoad() {

    memoryMappedNValues.setup("SettingsNumberValues", sizeof(int), false);
    isConnectedNValues = memoryMappedNValues.connect();
    if (isConnectedNValues) {
        nValues = memoryMappedNValues.getData();
    }

    memoryMappedValues.setup("SettingsValues", sizeof(float) * (*nValues) * 6, false);
    isConnectedValues = memoryMappedValues.connect();
    if (isConnectedValues) {
        values = memoryMappedValues.getData();
    }

    if (isConnectedNValues && isConnectedValues &&
        *nValues > 0) {

        NValues = *nValues;
        Values = new float[NValues * 6];
        memcpy(Values, values, sizeof(float) * NValues * 6);
        return true;
    }
    return false;
}
