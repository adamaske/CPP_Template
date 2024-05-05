from enum import Enum

packet_type_byte_amount = 4
packet_size_byte_amount = 4
incoming_packet_size_byte_amount = 4
current_byte_order = "big"
class PacketType(Enum):
    INVALID = 0
    STRING = 1
    INTEGER_ARRAY = 2
    TEST = 3

def PacketTypeToBytes(type=PacketType):
    return int.to_bytes(type.value, length=packet_type_byte_amount, byteorder=current_byte_order)
     
def PacketSizeToBytes(size=int):
    return int.to_bytes(size, length=packet_size_byte_amount, byteorder=current_byte_order)

def ParseIncomingPacketSize(size_bytes:bytes):
    return int.from_bytes(size_bytes, byteorder=current_byte_order)

def MakeStringPacket(msg=str):
    type_bytes = PacketTypeToBytes(PacketType.STRING)
    size_bytes = PacketSizeToBytes(len(msg))    
    content_bytes = msg.encode("UTF-8")

    packet_bytes = type_bytes + size_bytes
    packet_bytes = packet_bytes + content_bytes
    
    packet_size_bytes = int.to_bytes(len(packet_bytes), length=incoming_packet_size_byte_amount,byteorder=current_byte_order)
    return packet_size_bytes, packet_bytes

def ParsePacketType(packet_bytes):
    type_bytes = packet_bytes[:packet_type_byte_amount]
    type_int = int.from_bytes(type_bytes, byteorder=current_byte_order)
    return PacketType(type_int)

def ParsePacketSize(packet_bytes):
    size_bytes = packet_bytes[packet_type_byte_amount: packet_type_byte_amount + packet_size_byte_amount]
    size_int = int.from_bytes(size_bytes, byteorder=current_byte_order)
    return size_int

def ParsePacketContent(type=PacketType, packet_bytes=bytes):
    if type == PacketType.INVALID:
        return "Invalid"
    
    if type == PacketType.STRING:
        return packet_bytes[packet_type_byte_amount + packet_size_byte_amount :].decode("UTF-8")
    
    if type == PacketType.INTEGER_ARRAY:
        return "Integer Array"
        int_byte_amount = 4
        int_amount = int(len(content_bytes) / int_byte_amount)

        ints = []
        for i in range(int(int_amount)):
            ints.append(int.from_bytes(content_bytes[i * int_byte_amount : (i * int_byte_amount) + int_byte_amount]))
        print("ints", ints)
        
    if type == PacketType.TEST:
        return "Test"
    
def ParsePacket(packet_bytes=bytes):
    type = ParsePacketType(packet_bytes)
    size = ParsePacketSize(packet_bytes)
    content = ParsePacketContent(type, packet_bytes)
    
    print("PARSED PACKET : ")
    print("type : ", type)
    print("size : ", size)
    print("content : ", content)
    
    return type, size, content

