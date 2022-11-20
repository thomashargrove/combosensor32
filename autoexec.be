# Re-run this with Br load("autoexec.be")
# Or copy-paste into berry editor
# When rerunning, uncomment this line

# tasmota.remove_driver(d1)

# Notes on text output:
#         Height  Width  Cols
#  f0s1:  8       6      21
#  f0s2:  16      12     10
#
# small: 4+2+2 = 8x6 = 48
# big: 4+2+2 = 8x12 = 96

import json
import string

class ComboDisplay
  var prev_temperature, prev_humidity
  var co2, pm25, temperature, humidity
  var jsondata
  var big_prev_label, big_prev_num, small1_prev_label, small1_prev_num, small2_prev_label, small2_prev_num
  var mode, last_mode
  def init()
    self.prev_temperature = 0
    self.prev_humidity = 0

    self.big_prev_label = ""
    self.big_prev_num = 0
    self.small1_prev_label = ""
    self.small1_prev_num = 0
    self.small2_prev_label = ""
    self.small2_prev_num = 0

    self.mode = 0
    self.last_mode = 0
  end

  def display_big(label, num)
    # When we drop below 1000 we need to clear the first character for the label
    if ((num<1000) && (self.big_prev_num>=1000))
      tasmota.cmd("DisplayText [f4s2x0y16p-4]0")
      self.big_prev_label = ""
    end
    if (num != self.big_prev_num)
      if (num >= 1000)
        tasmota.cmd(string.format("DisplayText [f4s2x0y16p-4]%i", num))
      else
        tasmota.cmd(string.format("DisplayText [f4s2x32y16p-3]%i", num))
      end
      self.big_prev_num = num
    end
    if (label != self.big_prev_label && num < 1000)
      tasmota.cmd(string.format("DisplayText [f0s1x0y56]%s", label))
      self.big_prev_label = label
    end
  end

  def display_small1(label, num)
    if (label != self.small1_prev_label)
      tasmota.cmd(string.format("DisplayText [f0s1x0y8p3]%s", label))
      self.small1_prev_label = label
    end
    if (num != self.small1_prev_num)
      tasmota.cmd(string.format("DisplayText [f0s2x18y0p-4]%i", num))
      self.small1_prev_num = num
    end
  end

  def display_small2(label, num)
    if (label != self.small2_prev_label)
      tasmota.cmd(string.format("DisplayText [f0s1x72y8p3]%s", label))
      self.small2_prev_label = label
    end
    if (num != self.small2_prev_num)
      tasmota.cmd(string.format("DisplayText [f0s2x90y0p-3]%i", num))
      self.small2_prev_num = num
    end
  end

  def every_second()
    self.jsondata = json.load(tasmota.read_sensors(true))
    if (self.jsondata.contains("S8"))
      self.co2 = self.jsondata["S8"]["CarbonDioxide"]
    end
    if (self.jsondata.contains("PMS5003"))
      self.pm25 = self.jsondata["PMS5003"]["CF2.5"]
      self.temperature = int(self.jsondata["PMS5003"]["Temperature"])
      self.humidity = int(self.jsondata["PMS5003"]["Humidity"])
    end

    if (self.mode == 0)
      self.display_big("CO2", self.co2)
      self.display_small1("AQI", self.pm25)
      self.display_small2("%RH", self.humidity)
    elif (self.mode == 1)
      self.display_big("AQI", self.pm25)
      self.display_small1("CO2", self.co2)
      self.display_small2("%RH", self.humidity)
    end

  end
  def next_mode()
    log("Button Pressed")
    self.mode += 1
    if (self.mode >= 2)
      self.mode = 0
    end
  end
  def before_del()
    tasmota.remove_driver(self)
  end
end

d1 = ComboDisplay()
tasmota.add_driver(d1)

