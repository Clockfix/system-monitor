import psutil
import serial
import time
import serial.tools.list_ports

def sendx(comport, data):
    if comport == "NONE" :
        print("SEND to COM port FAIL")
    else :
        #print(comport)
        ser = serial.Serial(comport, 19200, timeout=0.5)
        ser.close()
        if ser.is_open :
            print("Port is OPEN")
        else:
            ser.open()
        n = 0
        while n < len(data)-1 :
            # print(str(data[n]))
            # ser.flushOutput() # Clear output buffer
            send_string = str(data[n]) +  "\n"
            n += 1
            ser.write(send_string)
            time.sleep(0.1)
        ser.close()

def get_data():
    cpu = psutil.cpu_percent(interval=1, percpu=True)
    mem = psutil.virtual_memory()
    swap = psutil.swap_memory()
    disk_usage = psutil.disk_usage('/home')
    disk_root = psutil.disk_usage('/')
    return "CPU0"+str(cpu[0]),"CPU1"+str(cpu[1]),"CPU2"+str(cpu[2]),"CPU3"+str(cpu[3]), "RAMM"+str(mem.percent), "SWAP"+str(swap.percent), "HOME"+str(disk_usage.percent), "ROOT"+str(disk_root.percent), "DONE"
  
def get_serial():
    ports = serial.tools.list_ports.comports()
    info = []
    for port, desc, hwid in sorted(ports):
        info +=  [ port, desc, hwid]
        # print("{}: {} [{}]".format(port, desc, hwid))
        # print(info)
    try:
        #ind = info.index("USB VID:PID=2341:0044 SER=6493633393635140C1B1 LOCATION=3-10:1.0")   # Arduino MEGA
        ind = info.index("USB VID:PID=2341:0001 SER=74132343530351F05052 LOCATION=3-10:1.0")    # Arduino UNO
        #print(info[ind-2])
        return info[ind-2]
    except ValueError:
        print("Arduino board not found")
        return "NONE"
    
# print(get_data())
# print(get_serial())

while True : 
    sendx( get_serial(), get_data() )
    time.sleep(0.5)














# # non-blocking (percentage since last call)
# cpu = psutil.cpu_percent(interval=1, percpu=True)
# print(cpu)
# print(cpu[0])
# print(cpu[1])
# print(cpu[2])
# print(cpu[3])
## blocking, per-cpu
# print('CPU usage = ' + str(psutil.cpu_percent(interval=None)) + '%')
#
# # Memory
# mem = psutil.virtual_memory()
# print(mem)
# ava = mem.available / (1024 * 1024 * 1024)
# tot = mem.total / (1024 * 1024 * 1024)
# print('Total mem = ' + str(round(tot, 2)) + ' G')
#print('Free mem = ' + str(ava))
# print(str(mem.percent) + '% Used')
#
# # SWAP memory
# swap = psutil.swap_memory()
# print(swap)
# print('Swap mem usage = ' + str(swap.percent) + '%')
#
## Disks
#
#disks =  psutil.disk_partitions()
#print(disks)
#
# disk_usage = psutil.disk_usage('/home')
#print(disk_usage)
# disk_tot = disk_usage.total / (1024 * 1024 * 1024)
# disk_free = disk_usage.free / (1024 * 1024 * 1024)
# print('Disk space = ' + str(round(disk_tot, 2)) + ' G')
# print('Used space = ' + str(disk_usage.percent) + '%')
# print('Free space = ' + str(round(disk_free, 2)) + ' G')
# print(' ')
# disk_usage = psutil.disk_usage('/')
# disk_tot = disk_usage.total / (1024 * 1024 * 1024)
# disk_free = disk_usage.free / (1024 * 1024 * 1024)
# print('Disk space = ' + str(round(disk_tot, 2)) + ' G')
# print('Used space = ' + str(disk_usage.percent) + '%')
# print('Free space = ' + str(round(disk_free, 2)) + ' G')
#
## Detecting COM ports
#
# # first sample
# comlist = serial.tools.list_ports.comports()
# connected = []
# for element in comlist:
#     connected.append(element.device)
# print("Connected COM ports: " + str(connected))
#
# second sample
# ports = serial.tools.list_ports.comports()
# info = []
# for port, desc, hwid in sorted(ports):
#         info +=  [ port, desc, hwid]
#         # print("{}: {} [{}]".format(port, desc, hwid))
#
# # print(info)
#
# try:
#     #ind = info.index("USB VID:PID=2341:0044 SER=6493633393635140C1B1 LOCATION=3-10:1.0")   # Arduino MEGA
#     ind = info.index("USB VID:PID=2341:0001 SER=74132343530351F05052 LOCATION=3-10:1.0")    # Arduino UNO
#     #print(info[ind-2])
# except ValueError:
#     print("Arduino board not found")
#
# sendx(str(info[ind-2]) , "CPU1"+str(cpu[0]))
# # therd sample
# ports = serial.tools.list_ports.comports()
# for port in ports :
#     print(port.device)

