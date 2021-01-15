# Overview
With a valid websocket connection, the following requests are available to you.

# Availables stations

## Request:
```json
{
    "id": 1
}
```

## Response:

```json
{
    "id": 1,
    "stations": [
        {
            "stationId": 1,
            "stationName": "WF"
        },
        {
            "stationId": 2,
            "stationName": "BS"
        }
    ]
}
```

## If no stations are registered:

```json
{
    "id": 1,
    "stations": []
}
```

# Weather Status

## Request:
```json
{
    "id": 0,
    "stationIds": [1, 2]
}
```

## Response:
```json
{
    "id": 0,
    "stationId": 1,
    "temperature": 28.3,
    "humidity": 64.9,
    "time": "2021-01-15 15:00:08"
}
```

