#! /bin/bash

path=`pwd`
cert_path=$path/cert
ca_path=$path/cert/ca
client_path=$path/cert/client
server_path=$path/cert/server

mkdir -p $cert_path
mkdir -p $ca_path
mkdir -p $client_path
mkdir -p $server_path

cd $ca_path
openssl genrsa -out cakey.pem 2048
openssl req -new -key cakey.pem -out ca.csr -subj "/C=CN/ST=CQ/L=CQ/O=soce/OU=soce/CN=soceCA"
openssl x509 -req -days 365 -sha1 -extensions v3_ca -signkey cakey.pem -in ca.csr -out  cacert.pem

cd $client_path
openssl genrsa -out key.pem 2048
openssl req -new -key key.pem -out client.csr -subj "/C=CN/ST=CQ/L=CQ/O=soce/OU=soce/CN=soce.client"
openssl x509 -req -days 365 -sha1 -extensions v3_req -CA ${ca_path}/cacert.pem -CAkey ${ca_path}/cakey.pem -CAserial ${ca_path}/ca.srl -CAcreateserial -in client.csr -out cert.pem

cd $server_path
openssl genrsa -out key.pem 2048
openssl req -new -key key.pem -out server.csr -subj "/C=CN/ST=CQ/L=CQ/O=soce/OU=soce/CN=soce.server"
openssl x509 -req -days 365 -sha1 -extensions v3_req -CA ${ca_path}/cacert.pem -CAkey ${ca_path}/cakey.pem -CAserial ${ca_path}/ca.srl -CAcreateserial -in server.csr -out cert.pem
