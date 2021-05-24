# Server SFTP
Server SFTP cu utilizator ”anonim” si utilizatori cu autentificare. Spatiu limitat (quoata) pentru utilizatori (ex. 2GB per utilizator). Permiteti
acces din exterior doar pe porturile necesare.

### Virtual Machine Setup

Am folosit platforma Google Cloud Platform pentru a implementa serverul nostru sftp. Cream o instanta VM numita 'sftp' cu regiunea in europe-west3 (Frankfurt), 
pe sistemul de operare Ubuntu, versiunea 20.04 LTS si un spatiu de stocare de 10GB. 

Dupa ce ne adaugam cheia publica in instanta, putem accesa serverul folosind SSH la IP-ul extern 35.246.181.166.


Primul lucru pe care il face este de a updata driverele masinii virtuale pentru a nu da de niciun bug ulterior.
Toate comenzile ulterioare sunt executate ca root, dar pot fi utilizate si cu `sudo` in fata.

```
apt update
apt upgrade
```

## SFTP Setup

Cream grupul 'sftp' si un utilizator ce va face parte din acest grup, folosind comenzile urmatoare:
```
groupadd sftp	
useradd -g sftp  user
```
* `-g` : grupul sftp;

Pentru a implementa un server SFTP avem nevoie sa modificam fisierul /etc/ssh/sshd_config.
```
nano /etc/ssh/sshd_config
```
Comentam urmatoarea linie de cod si o inlocuim astfel:
```
# Subsystem     sftp    /usr/lib/openssh/sftp-server
Subsystem 	sftp 	internal-sftp
```
La sfarsitul fisierului, dorim sa adaugam urmatoarele comenzi:
```
Match Group sftp
	ChrootDirectory /home/sftp/%u
	ForceCommand internal-sftp
	AllowTCPForwarding no
	X11Forwarding no
```
* `ChrootDirectory`:  Setam directorul de baza pentru fiecare user din grup ca fiind /home/sftp/*numele userului*;
* `%u`: Shortcut pentru *user* (nu cel creat de mine);
* `ForceCommand`: Forteaza comanda `internal-sftp`;

Iesim, salvam fisierul si restartam serviciul sshd pentru a avea loc schimbarile efectuate.
```
sudo service sshd restart
```
Ce ne-a mai ramas de facut, este sa cream folderele unde utilizatorii sftp o sa isi stocheze informatiile. Putem face asta cu instructiunea `mkdir`, 
unde cream manual fiecare folder. De asemenea, putem folosi intructiunea install pentru a crea mai multe directoare odata si sa aplicam in acelasi timp si restriciile.

```
install -d -o user -g sftp /home/sftp/user/upload	
```
* /upload: Fisierul unde utilizatorii isi stocheaza informatia;

Ii atribuim utilizatorului un director root, unde acesta va intra automat la fiecare logare in server.

```
usermod -d /home/sftp/user user	
```
* `-d` : default directory


## Restrictii

Pentru a nu permite accesarea informatiilor vulnerabile de catre utilizatori, absolut toate fisierele trebuie sa apartine owner-ului si grupului `root` 
(cu exceptia fisierului upload),inclusiv cele denumite dupa user. Astfel, utilizatorii nu pot iesii din fisierul lor root. 
Restrictiile trebuie sa arate in felul urmator.
```
drwxr-xr-x 4 root root 4096 May 22 20:33 ./
drwxr-xr-x 5 root root 4096 May 22 20:32 ../
drwxr-xr-x 3 root root 4096 May 24 11:10 anonymous/
drwxr-xr-x 4 root root 4096 May 24 11:10 user/
```
Restrictiile se pot modifica folosind comenzile:
```
chown root:root user
chmod 755 user
```

Singurul fisier ce nu trebuie sa-l aiba ca owner pe root, este fisierul unde userii isi stocheaza informatiile. Acestea trebuie sa arate astfel:
``` 
drwxr-xr-x 4 root root 4096 May 24 12:22 ./
drwxr-xr-x 4 root root 4096 May 24 12:22 ../
drwx------ 2 user sftp 4096 May 24 10:24 .ssh/
drwxr-xr-x 3 user sftp 4096 May 24 11:10 upload/
```
De asemenea, daca doriti ca fisierul upload sa nu fie accesat de niciun alt utilizator al serverului (in afara de root, iar userii sftp oricum nu pot ajunge acolo), 
puteti sa modificati restrictiile in felul urmator:
```
chown user:sftp upload  
chmod 700 upload
```

### User-ul Anonim

Deoarece prin cerintele proiectului se enumera si crearea unui user anonim, am creat acest user folosind aceeasi pasi anteriori. I-am setat o parola nula folosind 
instructiunea `` usermod -p "" anonymous``. Insa, pentru a folosi user-ul anonim, tot trebuie introdusa o cheie pubica numita anonymous, cheia publica fiind 
a dispozitivului de pe care doriti sa intrati.


## Quotele 


## Porturi

Pentru a putea vedea porturile deschise, ne vom folosim de net-tool.
```
apt install net-tools
netstat -tulpn
```
Dupa folosirea comenzii ''netstat -tulpn'' se vor afisa urmatoarele:
```
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name
tcp        0      0 127.0.0.53:53           0.0.0.0:*               LISTEN      436/systemd-resolve
tcp        0      0 0.0.0.0:22              0.0.0.0:*               LISTEN      718/sshd: /usr/sbin
tcp6       0      0 :::22                   :::*                    LISTEN      718/sshd: /usr/sbin
udp        0      0 127.0.0.53:53           0.0.0.0:*                           436/systemd-resolve
udp        0      0 10.156.0.6:68           0.0.0.0:*                           433/systemd-network
udp        0      0 127.0.0.1:323           0.0.0.0:*                           505/chronyd
udp6       0      0 ::1:323                 :::*                                505/chronyd
```

Porturile in state-ul LISTEN sunt:
```
  PORT		    Service				        Protocol		
- Portul 53: 	DNS (Domain Name Service) 	tcp
- Portul 22: 	SSH (Secure Shell)		    tcp
```

Dezactivam traficul HTTP si HTTPS din setarile masinii virtuale pentru a fi siguri ca accesarea se va
realiza doar pe porturile necesare.




