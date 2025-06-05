#include <Arduino.h>
#include <unity.h>
#include "WiFiManager.h"
#include "WiFi.h"

WiFiManager wifiManager;

// Fake Wi-Fi credentials for testing
const char *FAKE_SSID = "FakeSSID";
const char *FAKE_PASSWORD = "FakePassword";

// Real Wi-Fi credentials
const char *TEST_SSID = "Coron28";
const char *TEST_PASSWORD = "masa1222";

// IPアドレス形式チェック用ヘルパー関数
bool isValidIPAddress(const String &ip)
{
    int dotCount = 0;
    for (int i = 0; i < ip.length(); i++)
    {
        if (ip.charAt(i) == '.')
        {
            dotCount++;
        }
    }
    if (dotCount != 3)
        return false;

    int start = 0;
    for (int i = 0; i < 4; i++)
    {
        int dot = ip.indexOf('.', start);
        String part;
        if (dot == -1 && i == 3)
        {
            part = ip.substring(start);
        }
        else if (dot == -1)
        {
            return false;
        }
        else
        {
            part = ip.substring(start, dot);
        }
        if (part.length() == 0)
            return false;
        int num = part.toInt();
        if (num < 0 || num > 255)
            return false;
        start = dot + 1;
    }
    return true;
}

// Test: Wi-Fi接続タイムアウト
void test_wifi_connection_timeout()
{
    wifiManager.begin(FAKE_SSID, FAKE_PASSWORD);
    TEST_ASSERT_FALSE(wifiManager.isConnected());
}

// Test: 切断状態でのIPアドレス取得
void test_get_ip_address_disconnected()
{
    String ip = wifiManager.getIPAddress();
    TEST_ASSERT_EQUAL_STRING("Not Connected", ip.c_str());
}

// Test: Wi-Fiに正常接続できた場合のIPアドレス取得
void test_wifi_connection_success()
{
    WiFi.disconnect(); // 切断状態から開始
    delay(1000);       // 切断を待つ
    WiFi.begin(TEST_SSID, TEST_PASSWORD);
    wifiManager.begin(TEST_SSID, TEST_PASSWORD);

    TEST_ASSERT_TRUE(wifiManager.isConnected());
    String ip = wifiManager.getIPAddress();
    // 任意のローカルIPアドレスであればテスト成功
    TEST_ASSERT_TRUE(isValidIPAddress(ip));
}

void setup()
{
    UNITY_BEGIN();

    RUN_TEST(test_wifi_connection_timeout);
    RUN_TEST(test_get_ip_address_disconnected);
    RUN_TEST(test_wifi_connection_success);

    UNITY_END();
}

void loop()
{
    // Not used for unit testing
}