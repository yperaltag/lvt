FROM alpine

RUN apk update &&\
    apk add g++ make cmake grpc-dev gtest-dev postgresql-dev postgresql

COPY thirdparty/pistache/include/ /usr/include/
COPY thirdparty/pistache/lib/ /usr/lib/

COPY thirdparty/pqxx/include/ /usr/include/
COPY thirdparty/pqxx/lib/ /usr/lib/

RUN mkdir -p /usr/local/pgsql/data
RUN chown postgres /usr/local/pgsql/data

RUN mkdir /run/postgresql
RUN chown postgres /run/postgresql

RUN su postgres -c 'pg_ctl initdb -D /usr/local/pgsql/data' &&\
    su postgres -c 'pg_ctl start -D /usr/local/pgsql/data' &&\
    psql -U postgres -c "ALTER USER postgres WITH PASSWORD 'postgres';" &&\
    psql -U postgres -c "CREATE DATABASE lvt;" &&\
    psql -U postgres -d lvt -c "CREATE TABLE vehicle (id VARCHAR(50) PRIMARY KEY, latitude DOUBLE PRECISION NOT NULL, longitude DOUBLE PRECISION NOT NULL);" &&\
    su postgres -c 'pg_ctl stop -D /usr/local/pgsql/data'

WORKDIR /home/demo

COPY . .

WORKDIR /home/build

RUN cmake ../demo
RUN make

RUN ./http_pistache_test
RUN ./grpc_test
RUN su postgres -c 'pg_ctl start -D /usr/local/pgsql/data' &&\
    sleep 3 &&\
    ./postgresql_test &&\
    su postgres -c 'pg_ctl stop -D /usr/local/pgsql/data'

CMD su postgres -c 'pg_ctl start -D /usr/local/pgsql/data' && ./lvt_server

