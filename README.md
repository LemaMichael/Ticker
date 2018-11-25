# Ticker
Display Crypto Prices from Coinbase including BTC, ETH, LTC, BCH and ETC


## Requirements
- NodeMCU ESP8266
- MAX7219 Dot Matrix Module

## Demo

- Bitcoin price
 <p align="center">
 <img src = "/Demo/BTCDemo.gif" height = "350" >
</p>


- Ethereum Price
 <p align="center">
 <img src = "/Demo/ETHDemo.gif" height = "350" >
</p>

## Update 
Modify the following array with your coin balances to display your portfolio.

```c
float coinBalance[] {
  0.0, // BTC
  0.0, // ETH
  0.0, // LTC
  0.0, // BCH
  0.0 // ETC
};
```
If you don't want to display your portfolio, download this ino file [here](https://github.com/LemaMichael/Ticker/files/2613270/CoinbaseTicker-No-Portfolio.ino.zip)

## Get Started
- Enter your SSID and password of the Wi-Fi network in the [ino file](https://github.com/LemaMichael/Ticker/blob/ec17c1491a1fecc3da36ab58bc1a7bdb270c2c04/CoinbaseTicker/CoinbaseTicker.ino#L42-L43)
