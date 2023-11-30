
Create venv
- `python -m venv venv`


Activate venv

| Platform     | Shell       | Command                                     |
|--------------|-------------|---------------------------------------------|
| POSIX        | bash/zsh    | `$ source venv/bin/activate`                |
| POSIX        | fish        | `$ source venv/bin/activate.fish`           |
| POSIX        | csh/tcsh    | `$ source venv/bin/activate.csh`            |
| POSIX        | PowerShell  | `$ venv/bin/Activate.ps1`                   |
| Windows      | cmd.exe     | `C:\> venv\Scripts\activate.bat`            |
| PowerShell   | PowerShell  | `PS C:\> venv\Scripts\Activate.ps1`         |


Install requirements:
- `pip install -r requirements.txt`

Create Influxdb database
- Influxdb: https://www.influxdata.com/
- Create database bucket: https://docs.influxdata.com/influxdb/v2/admin/buckets/create-bucket/
- Add Python data source and get token, org, host and bucket
- Add token, org, host and bucket to coap_server.py

Copy and paste python servers ipv6 address to coap_client.py

Run the project
- `python main.py`

### Deactivate venv
- `deactivate`

### Remove venv
- `rm -rf venv`

