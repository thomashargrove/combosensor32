# Re-run this with Br load("autoexec.be")
# Or copy-paste into berry editor
# When rerunning, uncomment this line
#
#tasmota.remove_driver(comboDriver)

# Notes on text output:
#         Height  Width  Cols Rows
#  f0s1:  8       6      21   8
#  f0s2:  16      12     10   4


import json
import string

class ComboDisplay
  var co2, aqi, temperature, humidity
  var jsondata
  var big_prev_value, small1_prev_value, small2_prev_value
  var last_row_values
  var refresh
  # Mode 0 shows CO2 big; Mode1 shows AQI big
  var mode
  def init()
    self.co2 = 0
    self.aqi = 0
    self.temperature = 0
    self.humidity = 0
    self.big_prev_value = -1
    self.small1_prev_value = -1
    self.small2_prev_value = -1
    self.last_row_values = [-1,-1,-1,-1]
    self.refresh = true
    self.mode = 0
  end

  def display_big(label, value, refresh)
    if (refresh || (self.big_prev_value >= 1000 && value < 1000))
      tasmota.cmd(string.format("DisplayText [f0s1x0y56]%s", label))
    end

    # When we drop below 1000 we need to clear the first character for the label
    if ((value<1000) && (self.big_prev_value>=1000))
      tasmota.cmd("DisplayText [f4s2x0y16p-4]0")
      self.big_prev_value = ""
    end
    if (value != self.big_prev_value)
      if (value >= 1000)
        tasmota.cmd(string.format("DisplayText [f4s2x0y16p-4]%i", value))
      else
        tasmota.cmd(string.format("DisplayText [f4s2x32y16p-3]%i", value))
      end
      self.big_prev_value = value
    end
  end

  def display_small1(label, value, refresh)
    if (refresh)
      tasmota.cmd(string.format("DisplayText [f0s1x0y8p3]%s", label))
    end
    if (value != self.small1_prev_value)
      tasmota.cmd(string.format("DisplayText [f0s2x18y0p-4]%i", value))
      self.small1_prev_value = value
    end
  end

  def display_small2(label, value, refresh)
    if (refresh)
      tasmota.cmd(string.format("DisplayText [f0s1x72y8p3]%s", label))
    end
    if (value != self.small2_prev_value)
      tasmota.cmd(string.format("DisplayText [f0s2x90y0p-3]%i", value))
      self.small2_prev_value = value
    end
  end

  def display_row(position, label, value, refresh)
    if (refresh)
      tasmota.cmd(string.format("DisplayText [f0s2x0y%i]%s", position*16, label))
    end
    if (value != self.last_row_values[position])
      tasmota.cmd(string.format("DisplayText [f0s2x80y%ip-4]%i", position*16, value))
      self.last_row_values[position] = value
    end
  end
  
  def display_info(refresh)
    if (refresh)
      tasmota.cmd("DisplayText [z]")
      var hostname = tasmota.cmd("hostname")["Hostname"]
      tasmota.cmd(string.format("DisplayText [f0s2x0y0]%s", hostname)
      var ip = tasmota.wifi()["ip"]
      tasmota.cmd(string.format("DisplayText [f0s2x0y16]%s", hostname)
      var ip = tasmota.wifi()["ip"]
      tasmota.cmd(string.format("DisplayText [f0s2x0y16]%s", hostname)
    end
  end

  def display_off(refresh)
    if (refresh)
      tasmota.cmd("DisplayText [z]")
      tasmota.cmd("DisplayText [f0s1x0y0].")
    end
  end

  def every_second()
    if (self.refresh)
      tasmota.cmd("DisplayText [z]")
    end

    self.jsondata = json.load(tasmota.read_sensors(true))
    if (self.jsondata == nil) 
      log("nil json data")
      return
    end
    if (self.jsondata.contains("S8"))
      self.co2 = self.jsondata["S8"]["CarbonDioxide"]
    end
    if (self.jsondata.contains("PMS5003"))
      self.aqi = self.jsondata["PMS5003"]["CF2.5"]
      self.temperature = int(self.jsondata["PMS5003"]["Temperature"])
      self.humidity = int(self.jsondata["PMS5003"]["Humidity"])
    end
    if (self.mode == 0)
      self.display_big("CO2", self.co2, self.refresh)
      self.display_small1("AQI", self.aqi, self.refresh)
      self.display_small2("%RH", self.humidity, self.refresh)
    elif (self.mode == 1)
      self.display_big("AQI", self.aqi, self.refresh)
      self.display_small1("CO2", self.co2, self.refresh)
      self.display_small2("%RH", self.humidity, self.refresh)
    elif (self.mode == 2)
      self.display_row(0, "CO2", self.co2, self.refresh)
      self.display_row(1, "Aqi", self.aqi, self.refresh)
      self.display_row(2, "Temp", self.temperature, self.refresh)
      self.display_row(3, "%RH", self.humidity, self.refresh)
    elif (self.mode == 3)
      self.display_info(self.refresh)
    elif (self.mode == 4)
      self.display_off(self.refresh)
    end
    self.refresh = false
  end

  def next_mode()
    log("Button Pressed")
    self.mode += 1
    if (self.mode >= 5)
      self.mode = 0
    end

    self.big_prev_value = -1
    self.small1_prev_value = -1
    self.small2_prev_value = -1
    self.last_row_values = [-1,-1,-1,-1]
    self.refresh = true
  end
  
  def before_del()
    tasmota.remove_driver(self)
  end
end

comboDriver = ComboDisplay()
tasmota.add_driver(comboDriver)
