


class barometer:
    def barometer_calculate(self, baro_value, temperature):
        pressureHPA = baro_value / 4096
        print("Pressure: {}".format(pressureHPA))
        intermediate1 = pow((1001.1 / pressureHPA), 0.1902)
        intermediate2 = intermediate1 - 1
        intermediate3 = intermediate2 * (temperature + 273.15)
        relativeHeight = intermediate3 / 0.0065

        return relativeHeight

    def run(self):
        print("Started")
        s = "2A2928"  # DIT WERKT
        value_bar = int(s, 16)
        t = "9608"
        t2 = int(t, 16)
        value_temp = float(t2)/100
        level = self.barometer_calculate(float(value_bar),value_temp)
        print("Temp: {}".format(value_temp))
        print("Level: {}".format(level))

if __name__ == "__main__":
    barometer().run()
