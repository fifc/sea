* firewall-cmd --zone=FedoraServer --add-port=80/tcp --permanent
  zone: /etc/firewalld/firewalld.conf, 	DefaultZone=FedoraServer
* firewall-cmd --zone=FedoraServer --add-service=https --permanent
