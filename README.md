# Location vehicle tracking (lvt)

## Table of contents

<!--ts-->
   - [Build](#build)
   - [Run](#run)
   - [RPC Service](#rpc-service)
   - [HTTP Service](#http-service)
     * [Create vehicle](#create-vehicle)
     * [Get vehicle](#get-vehicle)
     * [Update vehicle](#update-vehicle)
     * [Remove vehicle](#remove-vehicle)
     * [Get vehicles in area](#get-vehicles-in-area)
<!--te-->

## Build
```
docker build . -t lvt
```

## Run
```
docker run --name demo -p 9080:9080 -p 50051:50051 -it lvt
```

## RPC Service

The RPC process is running at port 50051. Look at the **resources/protos/lvp.proto** file for the structure of the messages.

## HTTP Service

The HTTP process is running at port 9080. The request structure is as follows:

### Create vehicle

* Request:
`POST http://<host>:9080/v1/vehicles?latitude=<latitude>&longitude=<longitude>`
* Response:
A json file containing the vehicle id.
----------
   Example:
* Request:
`POST http://localhost:9080/v1/vehicles?latitude=1.2&longitude=3.4` 
* Response:
```json
{
        "id" : "1580228676813290700-0"
}
```
--------------
### Get vehicle

* Request:
`GET http://<host>:9080/v1/vehicles/<vehicleId>`
* Response:
A json file containing the vehicle.
----------
   Example:
* Request:
`GET http://localhost:9080/v1/vehicles/1580228676813290700-0`
* Response:
```json
{
        "id" : "1580228676813290700-0",
        "latitude" : 1.200000,
        "longitude" : 3.400000
}
```
---------------
### Update vehicle

* Request:
`PUT http://<host>:9080/v1/vehicles/<vehicleId>?latitude=<latitude>&longitude=<longitude>`
* Response:
Empty.
----------
   Example:
* Request:
`PUT http://localhost:9080/v1/vehicles/1580228676813290700-0?latitude=2.1&longitude=4.3`
-------------
### Remove vehicle

* Request:
`DELETE http://<host>:9080/v1/vehicles/<vehicleId>`
* Response:
Empty.
----------
   Example:
* Request:
`DELETE http://<host>:9080/v1/vehicles/1580228676813290700-0`
------------
### Get vehicles in area

* Request:
`GET http://<host>:9080/v1/vehiclesInArea?fromLatitude=<latitude>&toLatitude=<latitude>&fromLongitude=<longitude>&toLongitude=<longitude>`
* Response:
A json file containing the vehicle.
----------
   Example:
* Request:
`GET http://localhost:9080/v1/vehiclesInArea?fromLatitude=1.2&toLatitude=1.3&fromLongitude=3.4&toLongitude=3.5`
* Response:
```json
[
        {
            "id" : "1580228676813290700-0",
            "latitude" : 1.200000,
            "longitude" : 3.400000
        },
        {
            "id" : "1580230009296682000-1",
            "latitude" : 1.300000,
            "longitude" : 3.500000
        }
]
```
----------
