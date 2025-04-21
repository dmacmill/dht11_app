# dht11_app
An embedded project that uses the DHT11 to send temperature and humidity data 
up to a SSL/TLS socket server.

I wrote this to better familiarize myself with secure sockets in embedded 
projects, as well as to increase my familiarity with the esp32 ecosystem.

Later, I should have the data stored in DB and analyzable with matplotlib.

## How to SSL
SSL certificate stuff from the dht11_app_ssl_server is self certfied 
certificates from openssl.

Steps to reproduce the certs came from here:
https://gist.github.com/marshalhayes/ca9508f97d673b6fb73ba64a67b76ce8

really you just use
$ openssl req -x509 -newkey rsa:2048 -nodes -keyout server.key -out server.crt -days 365

Remember to use your local IP when prompted for "name"

you might need to go through that process again since cert expires
~April 19, 2026

```
"-----BEGIN CERTIFICATE-----\n" \
"MIIDizCCAnOgAwIBAgIUY3E3C9kfxcYD7Zpc88Sn9955O3wwDQYJKoZIhvcNAQEL\n" \
"BQAwVTELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAk1BMSEwHwYDVQQKDBhJbnRlcm5l\n" \
"dCBXaWRnaXRzIFB0eSBMdGQxFjAUBgNVBAMMDTE5Mi4xNjguMS4xNTIwHhcNMjUw\n" \
"NDE5MjMyNDA5WhcNMjYwNDE5MjMyNDA5WjBVMQswCQYDVQQGEwJVUzELMAkGA1UE\n" \
"CAwCTUExITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEWMBQGA1UE\n" \
"AwwNMTkyLjE2OC4xLjE1MjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"ALRXTOVDAB3OFWA31T4R9s+vpbjPXIot6NJhVdYPD/Ay+uaTfhiD3gGwyTX4X+8U\n" \
"bq0KmPXosZdtN/WdTxiL3+gGkf+Yaa2pmslJPN5yKvmG/qG95nDtevKBfyblnYGG\n" \
"MNHSIBWnBmP+mv0+wndV2seAENMmqWZ4F2lnhNfkXf24SZNAz30rY+lr/lz6tl87\n" \
"srULeKpxaotSpKaJFIlkkCukF6A+5a4ks1bnaCp49Qn7n2js5q0FU/3g1S8eeDlB\n" \
"W0vE62Yl8Z7jF/FZJxvmP4KCGRrsH6lxGolQHEugzOTHlMFx9A9dtA/FYKK4g8NB\n" \
"N5J7FnqFsB7pJFrcffnY8vcCAwEAAaNTMFEwHQYDVR0OBBYEFJPADdlo+JZ+DN/9\n" \
"506mLNUJIOg+MB8GA1UdIwQYMBaAFJPADdlo+JZ+DN/9506mLNUJIOg+MA8GA1Ud\n" \
"EwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAHyc5hDa2hoznwFBeffYMxL2\n" \
"xtGbDV5kA7tkUcahOv0gzYoU/GbKxeq4EstO1Hlc4XUeNwJ4iEGntE3hdqnSknIS\n" \
"1I306hlUTBy/yVmvB2tgRp/3+rQSvlcTYzQfSevijOL0zKAa82fDztFlAOoRNKLo\n" \
"Jw7/h5ZJlRSIKFQUEkIw7nj+NLhRq+gGPFCsHCfCSEAcHZeDf358XjT+dxPUOAKO\n" \
"ECSqtlpYssivRQMwfV4c1S7H92eag/VYApNf6NZuhtjeOC1MhPggWLjV/+XG2j2F\n" \
"29LV4nZwdhoDdvIYtnTGaiziz414e2icEGjkqxmaeEa5erykAIGwbavPUJB3qWQ=\n" \
"-----END CERTIFICATE-----\n"
```
Remember those \n's lol ask me how I know.