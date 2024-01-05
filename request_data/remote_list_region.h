// response example - {"regions": [{..."lat": 11.11, "lon": 22.22, "name": "Тестовый регион", "isActive": true}]}

typedef struct {
    float lat;
    float lon;
    String name;
    int timezone;
    bool isActive;
} region;