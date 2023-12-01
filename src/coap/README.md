# CoAP Server

## Instructions

1.  Clone the repository to your deployment server:

    - `git clone https://github.com/jsyrjako/Internet-of-Things.git`

2.  Change directory:

    - `cd Internet-of-Things/src/coap`

3.  Create venv

    - `python -m venv venv`

4.  Activate venv

    | Platform   | Shell      | Command                             |
    | ---------- | ---------- | ----------------------------------- |
    | POSIX      | bash/zsh   | `$ source venv/bin/activate`        |
    | POSIX      | fish       | `$ source venv/bin/activate.fish`   |
    | POSIX      | csh/tcsh   | `$ source venv/bin/activate.csh`    |
    | POSIX      | PowerShell | `$ venv/bin/Activate.ps1`           |
    | Windows    | cmd.exe    | `C:\> venv\Scripts\activate.bat`    |
    | PowerShell | PowerShell | `PS C:\> venv\Scripts\Activate.ps1` |

5.  Install requirements:

    - `pip install -r requirements.txt`

6.  Copy and paste server's ipv6 address to:

    - [config.py](config.py)
    - [/src/sensors/start.sh](/src/sensors/start.sh)

7.  Create InfluxDB database

    - [Influxdb](https://www.influxdata.com/)
    - Create database bucket [here](https://docs.influxdata.com/influxdb/v2/admin/buckets/create-bucket/)
    - Add Python data source and get token, org, host and bucket
    - Add token, org, host and bucket to [config.py](config.py)

8.  Run the project
    - `python coap_server.py`

#### Deactivate venv

- `deactivate`

#### Remove venv

- `rm -rf venv`
