// C:\Users\Michael\Desktop\test\ds_pc.h


char ReportDescriptor[74] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x35, 0x00,                    //   PHYSICAL_MINIMUM (0)
    0x45, 0x01,                    //   PHYSICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x0d,                    //   REPORT_COUNT (13)
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x0d,                    //   USAGE_MAXIMUM (Button 13)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x95, 0x03,                    //   REPORT_COUNT (3)
    0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
    0x25, 0x07,                    //   LOGICAL_MAXIMUM (7)
    0x46, 0x3b, 0x01,              //   PHYSICAL_MAXIMUM (315)
    0x75, 0x04,                    //   REPORT_SIZE (4)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x65, 0x14,                    //   UNIT (Eng Rot:Angular Pos)
    0x09, 0x39,                    //   USAGE (Hat switch)
    0x81, 0x42,                    //   INPUT (Data,Var,Abs,Null)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
    0x45, 0x02,                    //   PHYSICAL_MAXIMUM (2)
    0x25, 0x02,                    //   LOGICAL_MAXIMUM (2)
    0x75, 0x02,                    //   REPORT_SIZE (2)
    0x95, 0x04,                    //   REPORT_COUNT (4)
    0x09, 0x30,                    //   USAGE (X)
    0x09, 0x31,                    //   USAGE (Y)
    0x09, 0x32,                    //   USAGE (Z)
    0x09, 0x35,                    //   USAGE (Rz)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

