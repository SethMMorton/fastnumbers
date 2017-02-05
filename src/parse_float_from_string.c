/* See if a string contains a python float, and return the contained double. */
/* It is assumed that leading whitespace has already been removed. */
#include <Python.h>
#include <limits.h>
#include <float.h>
#include "parsing.h"
#include "quick_detection.h"


static double
power_of_ten_scaling_factor(const int expon);

static double
apply_power_of_ten_scaling(const double value, const int expon);

double
parse_float_from_string (const char *str, const char *end, bool *error)
{
    register long sign = 1L;
    register unsigned long intvalue = 0UL;
    register bool valid = false;
    register int expon = 0;
    register double value = 0.0;
    register const unsigned starts_with_sign = (unsigned) is_sign(str);

    *error = true;

    /* If we had started with a sign, increment the pointer by one. */

    sign = starts_with_sign && is_negative_sign(str) ? -1L : 1L;
    str += starts_with_sign;

    /* Otherwise parse as an actual number. */

    for (intvalue = 0UL; is_valid_digit(str); valid = true, str++) {
        intvalue *= 10UL;
        intvalue += ascii2ulong(str);
    }
    value = (double) intvalue;

    /* If long literal, quit here. */

    if (consume_python2_long_literal_lL(str)) {
        *error = !valid || str != end;
        return sign * value;
    }

    /* Parse decimal part. */

    if (is_decimal(str)) {
        register unsigned long decimal = 0UL;
        str++;
        for (expon = 0;
             is_valid_digit(str);
             valid = true, str++, expon++)
        {
            decimal *= 10UL;
            decimal += ascii2ulong(str);
        }
        value += apply_power_of_ten_scaling(decimal, -expon);
    }
 
    /* Parse exponential part. */

    if (is_e_or_E(str) && valid) {
        const int exp_sign = ++str &&
                             consume_sign_and_is_negative(str) ? -1 : 1;
        valid = false;
        for (expon = 0; is_valid_digit(str); valid = true, str++) {
            expon *= 10;
            expon += ascii2int(str);
        }
        value = apply_power_of_ten_scaling(value, exp_sign * expon);
    }

    *error = !valid || str != end;
    return sign * value;
}

double
apply_power_of_ten_scaling(const double value, const int expon)
{
    const double scale = power_of_ten_scaling_factor(abs(expon));
    return expon < 0 ? value / scale : value * scale;
}

double
power_of_ten_scaling_factor(const int expon) {
    /* Calculates the exponential scaling factor with hard-coded values. */
    switch(expon) {
    case 0:    return 1E0;
    case 1:    return 1E1;
    case 2:    return 1E2;
    case 3:    return 1E3;
    case 4:    return 1E4;
    case 5:    return 1E5;
    case 6:    return 1E6;
    case 7:    return 1E7;
    case 8:    return 1E8;
    case 9:    return 1E9;
    case 10:   return 1E10;
    case 11:   return 1E11;
    case 12:   return 1E12;
    case 13:   return 1E13;
    case 14:   return 1E14;
    case 15:   return 1E15;
    case 16:   return 1E16;
    case 17:   return 1E17;
    case 18:   return 1E18;
    case 19:   return 1E19;
    case 20:   return 1E20;
    case 21:   return 1E21;
    case 22:   return 1E22;
    case 23:   return 1E23;
    case 24:   return 1E24;
    case 25:   return 1E25;
    case 26:   return 1E26;
    case 27:   return 1E27;
    case 28:   return 1E28;
    case 29:   return 1E29;
    case 30:   return 1E30;
    case 31:   return 1E31;
    case 32:   return 1E32;
    case 33:   return 1E33;
    case 34:   return 1E34;
    case 35:   return 1E35;
    case 36:   return 1E36;
    case 37:   return 1E37;
    case 38:   return 1E38;
    case 39:   return 1E39;
    case 40:   return 1E40;
    case 41:   return 1E41;
    case 42:   return 1E42;
    case 43:   return 1E43;
    case 44:   return 1E44;
    case 45:   return 1E45;
    case 46:   return 1E46;
    case 47:   return 1E47;
    case 48:   return 1E48;
    case 49:   return 1E49;
    case 50:   return 1E50;
    case 51:   return 1E51;
    case 52:   return 1E52;
    case 53:   return 1E53;
    case 54:   return 1E54;
    case 55:   return 1E55;
    case 56:   return 1E56;
    case 57:   return 1E57;
    case 58:   return 1E58;
    case 59:   return 1E59;
    case 60:   return 1E60;
    case 61:   return 1E61;
    case 62:   return 1E62;
    case 63:   return 1E63;
    case 64:   return 1E64;
    case 65:   return 1E65;
    case 66:   return 1E66;
    case 67:   return 1E67;
    case 68:   return 1E68;
    case 69:   return 1E69;
    case 70:   return 1E70;
    case 71:   return 1E71;
    case 72:   return 1E72;
    case 73:   return 1E73;
    case 74:   return 1E74;
    case 75:   return 1E75;
    case 76:   return 1E76;
    case 77:   return 1E77;
    case 78:   return 1E78;
    case 79:   return 1E79;
    case 80:   return 1E80;
    case 81:   return 1E81;
    case 82:   return 1E82;
    case 83:   return 1E83;
    case 84:   return 1E84;
    case 85:   return 1E85;
    case 86:   return 1E86;
    case 87:   return 1E87;
    case 88:   return 1E88;
    case 89:   return 1E89;
    case 90:   return 1E90;
    case 91:   return 1E91;
    case 92:   return 1E92;
    case 93:   return 1E93;
    case 94:   return 1E94;
    case 95:   return 1E95;
    case 96:   return 1E96;
    case 97:   return 1E97;
    case 98:   return 1E98;
    case 99:   return 1E99;
    case 100:  return 1E100;
    case 101:  return 1E101;
    case 102:  return 1E102;
    case 103:  return 1E103;
    case 104:  return 1E104;
    case 105:  return 1E105;
    case 106:  return 1E106;
    case 107:  return 1E107;
    case 108:  return 1E108;
    case 109:  return 1E109;
    case 110:  return 1E110;
    case 111:  return 1E111;
    case 112:  return 1E112;
    case 113:  return 1E113;
    case 114:  return 1E114;
    case 115:  return 1E115;
    case 116:  return 1E116;
    case 117:  return 1E117;
    case 118:  return 1E118;
    case 119:  return 1E119;
    case 120:  return 1E120;
    case 121:  return 1E121;
    case 122:  return 1E122;
    case 123:  return 1E123;
    case 124:  return 1E124;
    case 125:  return 1E125;
    case 126:  return 1E126;
    case 127:  return 1E127;
    case 128:  return 1E128;
    case 129:  return 1E129;
    case 130:  return 1E130;
    case 131:  return 1E131;
    case 132:  return 1E132;
    case 133:  return 1E133;
    case 134:  return 1E134;
    case 135:  return 1E135;
    case 136:  return 1E136;
    case 137:  return 1E137;
    case 138:  return 1E138;
    case 139:  return 1E139;
    case 140:  return 1E140;
    case 141:  return 1E141;
    case 142:  return 1E142;
    case 143:  return 1E143;
    case 144:  return 1E144;
    case 145:  return 1E145;
    case 146:  return 1E146;
    case 147:  return 1E147;
    case 148:  return 1E148;
    case 149:  return 1E149;
    case 150:  return 1E150;
    case 151:  return 1E151;
    case 152:  return 1E152;
    case 153:  return 1E153;
    case 154:  return 1E154;
    case 155:  return 1E155;
    case 156:  return 1E156;
    case 157:  return 1E157;
    case 158:  return 1E158;
    case 159:  return 1E159;
    case 160:  return 1E160;
    case 161:  return 1E161;
    case 162:  return 1E162;
    case 163:  return 1E163;
    case 164:  return 1E164;
    case 165:  return 1E165;
    case 166:  return 1E166;
    case 167:  return 1E167;
    case 168:  return 1E168;
    case 169:  return 1E169;
    case 170:  return 1E170;
    case 171:  return 1E171;
    case 172:  return 1E172;
    case 173:  return 1E173;
    case 174:  return 1E174;
    case 175:  return 1E175;
    case 176:  return 1E176;
    case 177:  return 1E177;
    case 178:  return 1E178;
    case 179:  return 1E179;
    case 180:  return 1E180;
    case 181:  return 1E181;
    case 182:  return 1E182;
    case 183:  return 1E183;
    case 184:  return 1E184;
    case 185:  return 1E185;
    case 186:  return 1E186;
    case 187:  return 1E187;
    case 188:  return 1E188;
    case 189:  return 1E189;
    case 190:  return 1E190;
    case 191:  return 1E191;
    case 192:  return 1E192;
    case 193:  return 1E193;
    case 194:  return 1E194;
    case 195:  return 1E195;
    case 196:  return 1E196;
    case 197:  return 1E197;
    case 198:  return 1E198;
    case 199:  return 1E199;
    case 200:  return 1E200;
    case 201:  return 1E201;
    case 202:  return 1E202;
    case 203:  return 1E203;
    case 204:  return 1E204;
    case 205:  return 1E205;
    case 206:  return 1E206;
    case 207:  return 1E207;
    case 208:  return 1E208;
    case 209:  return 1E209;
    case 210:  return 1E210;
    case 211:  return 1E211;
    case 212:  return 1E212;
    case 213:  return 1E213;
    case 214:  return 1E214;
    case 215:  return 1E215;
    case 216:  return 1E216;
    case 217:  return 1E217;
    case 218:  return 1E218;
    case 219:  return 1E219;
    case 220:  return 1E220;
    case 221:  return 1E221;
    case 222:  return 1E222;
    case 223:  return 1E223;
    case 224:  return 1E224;
    case 225:  return 1E225;
    case 226:  return 1E226;
    case 227:  return 1E227;
    case 228:  return 1E228;
    case 229:  return 1E229;
    case 230:  return 1E230;
    case 231:  return 1E231;
    case 232:  return 1E232;
    case 233:  return 1E233;
    case 234:  return 1E234;
    case 235:  return 1E235;
    case 236:  return 1E236;
    case 237:  return 1E237;
    case 238:  return 1E238;
    case 239:  return 1E239;
    case 240:  return 1E240;
    case 241:  return 1E241;
    case 242:  return 1E242;
    case 243:  return 1E243;
    case 244:  return 1E244;
    case 245:  return 1E245;
    case 246:  return 1E246;
    case 247:  return 1E247;
    case 248:  return 1E248;
    case 249:  return 1E249;
    case 250:  return 1E250;
    case 251:  return 1E251;
    case 252:  return 1E252;
    case 253:  return 1E253;
    case 254:  return 1E254;
    case 255:  return 1E255;
    /* We should never see anything larger than 255. */
    /*
    case 256:  return 1E256;
    case 257:  return 1E257;
    case 258:  return 1E258;
    case 259:  return 1E259;
    case 260:  return 1E260;
    case 261:  return 1E261;
    case 262:  return 1E262;
    case 263:  return 1E263;
    case 264:  return 1E264;
    case 265:  return 1E265;
    case 266:  return 1E266;
    case 267:  return 1E267;
    case 268:  return 1E268;
    case 269:  return 1E269;
    case 270:  return 1E270;
    case 271:  return 1E271;
    case 272:  return 1E272;
    case 273:  return 1E273;
    case 274:  return 1E274;
    case 275:  return 1E275;
    case 276:  return 1E276;
    case 277:  return 1E277;
    case 278:  return 1E278;
    case 279:  return 1E279;
    case 280:  return 1E280;
    case 281:  return 1E281;
    case 282:  return 1E282;
    case 283:  return 1E283;
    case 284:  return 1E284;
    case 285:  return 1E285;
    case 286:  return 1E286;
    case 287:  return 1E287;
    case 288:  return 1E288;
    case 289:  return 1E289;
    case 290:  return 1E290;
    case 291:  return 1E291;
    case 292:  return 1E292;
    case 293:  return 1E293;
    case 294:  return 1E294;
    case 295:  return 1E295;
    case 296:  return 1E296;
    case 297:  return 1E297;
    case 298:  return 1E298;
    case 299:  return 1E299;
    case 300:  return 1E300;
    case 301:  return 1E301;
    case 302:  return 1E302;
    case 303:  return 1E303;
    case 304:  return 1E304;
    case 305:  return 1E305;
    case 306:  return 1E306;
    case 307:  return 1E307;
    case 308:  return 1E308;
    */
    /* This should never be reached. */
    default: return 1E308;
    }
}
