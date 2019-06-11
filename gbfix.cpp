
/* gbfix - GameBoy Header Fixer
 * By el_seyf
*/

using namespace std;

#include <iostream>
#include <fstream>
#include <vector>

const string options_text = string("gbfix - GameBoy Header Fixer, by el_seyf\n") +
                            string("        Fixes GameBoy ROMs internal Header Data\n") +
                            string("Usage: gbfix [options] \"in_file\"\n\n") + 
                            string("Options:\n") +
                            string("\t-h\t\tShow this help and quit\n") +
                            string("\t-i <File>\tUse <File> as Input\n") +
                            string("\t-o <File>\tOutput file to <File> (Default: <in_file>_fix)\n") +
                            string("\t-n <Name>\tUse <Name> as Header Name\n"
                                          "\t\t\t\t(up to 15 characters) (Default: \"\")\n") +
                            string("\t-gbc\t\tSet GBC Flag in Header (Default: False)\n") +
                            string("\t-l <Code>\tUse <Code> as Header License Code\n"
                                          "\t\t\t\t(Dec (N) or Hex (0xNN)) (Default: 0)\n") +
                            string("\t-c <Type>\tUse <Type> as Header Cartridge Type\n"
                                          "\t\t\t\t(Dec (N) or Hex (0xNN)) (Default: 0)\n") +
                            string("\t-rom <Banks>\tUse <Banks> as Header Rom Banks Number\n"
                                             "\t\t\t\t(Dec (N) or Hex (0xNN)) (Default: 0)\n") +
                            string("\t-ram <Size>\tUse <Size> as Header Ram Size\n"
                                            "\t\t\t\t(Valid <Size>: 0k, 2k, 8k, 32k, 64k, 128k) (Default: 0k)\n");

const uint8_t gb_logo[48] = {
    0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d,
    0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
    0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99,
    0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc,
    0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e
};

ifstream in;
ofstream out;

void apply_gb_header(uint8_t* _rom, uint32_t _rom_size, string _rom_name = "", bool _gbc = false, uint8_t _lic_code = 0x00,
                     uint8_t _cart_type = 0x00, uint16_t _rom_banks = 0x0000, uint8_t _ram_size = 0x00){
    //Nintendo Logo at 0x104 - 0x133
    for(int i = 0; i < sizeof(gb_logo); i++) _rom[0x104 + i] = gb_logo[i];
    //ROM Internal Name at 0x134 - 0x142 (15 Characters)
    for(int i = 0; i < 15; i++){
        if(i < _rom_name.length())
            _rom[0x134 + i] = (_rom_name[i] = toupper(_rom_name[i]));
        else
            _rom[0x134 + i] = 0x00;
    }
    _rom_name.resize(15);
    //CGB Flag at 0x143
    _rom[0x143] = _gbc ? 0x80 : 0x00;
    //New License Code at 0x144 - 0x145 (convert _lic_code to hex string)
    char _lic_code_c[4];
    sprintf(_lic_code_c, "%02X", _lic_code);
    _rom[0x144] = _lic_code_c[0];
    _rom[0x145] = _lic_code_c[1];
    //SGB Flag at 0x146, always 0x00
    _rom[0x146] = 0x00;
    //Cart Type at 0x147
    _rom[0x147] = _cart_type;
    //ROM Banks at 0x148, (log10(_rom_banks) / log10(2))
    _rom[0x148] = _rom_banks;
    //RAM Size at 0x149, arbitrary:
    _rom[0x149] = _ram_size;
    //Destination Code at 0x14A, always 0x01
    _rom[0x14A] = 0x01;
    //Old license Code at 0x14B, always 0x33
    _rom[0x14B] = 0x33;
    //Mask Rom Version at 0x14C, always 0x00
    _rom[0x14C] = 0x00;
    //Header Checksum at 0x14D, Checksum of Bytes 0x134 - 0x14C (24 Bytes)
    uint8_t _chksum = -1;
    for(int i = 0; i < 24; i++) _chksum = _chksum + ~_rom[0x134 + i];
    _rom[0x14D] = _chksum;
    //Global Checksum at 0x14E - 0x14F, calc from 0x0000 - 0x014D and 0x150 - end of Rom
    uint16_t _chksum16 = 0;
    for(int i = 0x0000; i < 0x014E; i++)    _chksum16 += _rom[i];
    for(int i = 0x0150; i < _rom_size; i++) _chksum16 += _rom[i];
    _rom[0x14E] = (_chksum16 >> 8) & 0xFF;
    _rom[0x14F] = (_chksum16 >> 0) & 0xFF;
    //String holding printable Header information:
    char _header_information[1024];
    sprintf(_header_information, "ROM Header Information:\n"
                                 "\tROM File Size:\t\t%i Bytes\n"
                                 "\tROM Name:\t\t%s\n"
                                 "\tGBC Flag:\t\t%s\n"
                                 "\tLicense Code:\t\t0x%02X\n"
                                 "\tCart Type:\t\t0x%02X\n"
                                 "\tROM Banks:\t\t0x%02X\n"
                                 "\tRAM Size:\t\t0x%02X\n"
                                 "\tHeader Checksum:\t0x%02X\n"
                                 "\tROM Checksum:\t\t0x%04X\n",
                                 _rom_size,
                                 _rom_name.c_str(),
                                 _gbc ? "Yes" : "No",
                                 _lic_code,
                                 _cart_type,
                                 _rom_banks,
                                 _ram_size,
                                 _chksum,
                                 _chksum16);
    cout << _header_information << endl;
}

int main(int argc, char** argv){
    if(argc < 2){
        cout << options_text << endl;
        return 0;
    }
    
    //Parse Parameters:
    string _in_path = argv[argc - 1];
    string _out_path = _in_path;
    if(_out_path.rfind(".gb") != string::npos)
        _out_path.insert(_out_path.rfind(".gb"), "_fix");
    bool _o_flag = false;
    
    string _rom_name = "";
    bool _gbc = false;
    uint8_t  _lic_code  = 0x00;
    uint8_t  _cart_type = 0x00;
    uint16_t _rom_banks = 0x0000;
    uint8_t  _ram_size  = 0x00;
    
    for(int i = 1; i < argc; i++){
        if(string(argv[i]) == string("-h")){
            cout << options_text << endl;
            return 0;
        }
        else if(string(argv[i]) == string("-i")){
            _in_path = argv[i + 1];
            _out_path = _in_path;
            if(_out_path.rfind(".gb") != string::npos)
                _out_path.insert(_out_path.rfind(".gb"), "_fix");
        }
        else if(string(argv[i]) == string("-o")){
            _out_path = argv[i + 1];
            _o_flag = true;
        }
        else if(string(argv[i]) == string("-n")){
            _rom_name = argv[i + 1];
        }
        else if(string(argv[i]) == string("-gbc")){
            _gbc = true;
        }
        else if(string(argv[i]) == string("-l")){
            try{ if(string(argv[i + 1]).find("0x") == 0)    _lic_code = stoi(argv[i + 1], nullptr, 16);
                 else                                       _lic_code = stoi(argv[i + 1], nullptr, 10);
            } catch(invalid_argument &e){cout << "Error: Invalid License Code (-l)..." << endl; exit(-1);}
        }
        else if(string(argv[i]) == string("-c")){
            try{ if(string(argv[i + 1]).find("0x") == 0)    _cart_type = stoi(argv[i + 1], nullptr, 16);
                 else                                       _cart_type = stoi(argv[i + 1], nullptr, 10);
            } catch(invalid_argument &e){cout << "Error: Invalid Cartridge Type (-c)..." << endl; exit(-1);}
        }
        else if(string(argv[i]) == string("-rom")){
            try{ if(string(argv[i + 1]).find("0x") == 0)    _rom_banks = stoi(argv[i + 1], nullptr, 16);
                 else                                       _rom_banks = stoi(argv[i + 1], nullptr, 10);
            } catch(invalid_argument &e){cout << "Error: Invalid ROM Banks (-rom)..." << endl; exit(-1);}
        }
        else if(string(argv[i]) == string("-ram")){
            string _ram_size_str = argv[i + 1];
            if(_ram_size_str.find("k") == (_ram_size_str.length() - 1)) _ram_size_str.resize(_ram_size_str.length() - 1);
            try{ if(_ram_size_str.find("0x") == 0)  _ram_size = stoi(_ram_size_str.c_str(), nullptr, 16);
                 else                               _ram_size = stoi(_ram_size_str.c_str(), nullptr, 10);
            } catch(invalid_argument &e){cout << "Error: Invalid RAM Size (-ram)..." << endl; exit(-1);}
        }
    }
    
    in.open(_in_path, ios::binary);
    if(!in.is_open()){
        cout << "Error: Could not open " << _in_path << "..." << endl;
        exit(-1);
    }
    
    //Read file:
    in.seekg(0, in.end);
    uint32_t _size = in.tellg();
    in.seekg(0, in.beg);
    uint8_t _rom[_size];
    in.read((char*)&_rom, sizeof(_rom));
    in.close();
    
    //Apply Gameboy Header:
    apply_gb_header(_rom, sizeof(_rom), _rom_name, _gbc, _lic_code, _cart_type, _rom_banks, _ram_size);
    
    out.open(_out_path, ios::binary | ios::trunc);
    if(!out.is_open()){
        cout << "Error: Could not open " << _out_path << "..." << endl;
        exit(-1);
    }
    out.write((char*)&_rom, sizeof(_rom));
    out.close();
    
    return 0;
}
