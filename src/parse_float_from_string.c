/* See if a string contains a python float, and return the contained double. */
#include <Python.h>
#include <limits.h>
#include <float.h>
#include "parsing.h"
#include "fast_conversions.h"

static long double
power_of_ten_scaling_factor(const int expon);

static long double
apply_power_of_ten_scaling(const long double value, const int expon);

static bool
check_for_overflow(const unsigned long value, const unsigned long cur_val);

double
parse_float_from_string (const char *str, bool *error, bool *overflow)
{
    register long sign = 1L;
    register unsigned long intvalue = 0UL;
    register bool valid = false;
    register int ndigits = 0;
    register int expon = 0;
    register long double value = 0.0L;
    *overflow = false;
    *error = true;

    consume_white_space(str);
    sign = consume_sign_and_is_negative(str) ? -1L : 1L;

    /* Are we possibly dealing with infinity or NAN? */

    if (is_n_or_N(str) || is_i_or_I(str)) {
        
        if (case_insensitive_match(str, "inf")) {
            str += 3;
            if (case_insensitive_match(str, "inity"))
                str += 5;
            *error = !trailing_characters_are_vaild_and_nul_terminated(&str);
            return sign * Py_HUGE_VAL;
        }

        else if (case_insensitive_match(str, "nan")) {
            str += 3;
            *error = !trailing_characters_are_vaild_and_nul_terminated(&str);
            return Py_NAN;
        }

    }

    /* Otherwise parse as an actual number. */

    for (intvalue = 0UL; is_valid_digit(str); valid = true, ndigits++, str++) {
        const unsigned long tmpval = ascii2ulong(str);
        *overflow = *overflow || check_for_overflow(intvalue, tmpval);
        intvalue *= 10L;
        intvalue += tmpval;
    }
    value = (long double) intvalue;

    /* If long literal, quit here. */

    if (consume_python2_long_literal_lL(str)) {
        *error = !valid ||
                 !trailing_characters_are_vaild_and_nul_terminated(&str);
        *overflow = *overflow || (value > DBL_MAX);
        return (double) (sign * value);
    }

    /* Parse decimal part. */

    if (is_decimal(str)) {
        register unsigned long decimal = 0UL;
        str++;
        for (expon = 0;
             is_valid_digit(str);
             valid = true, str++, ndigits++, expon++)
        {
            const unsigned long tmpval = ascii2ulong(str);
            *overflow = *overflow || check_for_overflow(decimal, tmpval);
            decimal *= 10L;
            decimal += tmpval;
        }
        /* Too many digits loses precision */
        *overflow = *overflow || (ndigits >= DBL_DIG - 1);
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
        /* Exponent > 255 is unreliable */
        *overflow = *overflow || (expon > 255);
        value = apply_power_of_ten_scaling(value, exp_sign * expon);
    }

    *error = !valid || !trailing_characters_are_vaild_and_nul_terminated(&str);
    *overflow = *overflow || (value > DBL_MAX);  /* One last overflow check */
    return (double) (sign * value);
}

bool
check_for_overflow(const unsigned long value, const unsigned long cur_val)
{
    static const unsigned long overflow_cutoff = ULONG_MAX / 10UL;
    static const unsigned long overflow_last_digit_limit = ULONG_MAX % 10UL;
    return value > overflow_cutoff ||
          (value == overflow_cutoff && cur_val > overflow_last_digit_limit);
}

long double
apply_power_of_ten_scaling(const long double value, const int expon)
{
    const long double scale = power_of_ten_scaling_factor(abs(expon));
    return expon < 0 ? value / scale : value * scale;
}

long double
power_of_ten_scaling_factor(const int expon) {
    /* Calculates the exponential scaling factor with hard-coded values. */
    switch(expon) {
    case 0:    return 1E0L;
    case 1:    return 1E1L;
    case 2:    return 1E2L;
    case 3:    return 1E3L;
    case 4:    return 1E4L;
    case 5:    return 1E5L;
    case 6:    return 1E6L;
    case 7:    return 1E7L;
    case 8:    return 1E8L;
    case 9:    return 1E9L;
    case 10:   return 1E10L;
    case 11:   return 1E11L;
    case 12:   return 1E12L;
    case 13:   return 1E13L;
    case 14:   return 1E14L;
    case 15:   return 1E15L;
    case 16:   return 1E16L;
    case 17:   return 1E17L;
    case 18:   return 1E18L;
    case 19:   return 1E19L;
    case 20:   return 1E20L;
    case 21:   return 1E21L;
    case 22:   return 1E22L;
    case 23:   return 1E23L;
    case 24:   return 1E24L;
    case 25:   return 1E25L;
    case 26:   return 1E26L;
    case 27:   return 1E27L;
    case 28:   return 1E28L;
    case 29:   return 1E29L;
    case 30:   return 1E30L;
    case 31:   return 1E31L;
    case 32:   return 1E32L;
    case 33:   return 1E33L;
    case 34:   return 1E34L;
    case 35:   return 1E35L;
    case 36:   return 1E36L;
    case 37:   return 1E37L;
    case 38:   return 1E38L;
    case 39:   return 1E39L;
    case 40:   return 1E40L;
    case 41:   return 1E41L;
    case 42:   return 1E42L;
    case 43:   return 1E43L;
    case 44:   return 1E44L;
    case 45:   return 1E45L;
    case 46:   return 1E46L;
    case 47:   return 1E47L;
    case 48:   return 1E48L;
    case 49:   return 1E49L;
    case 50:   return 1E50L;
    case 51:   return 1E51L;
    case 52:   return 1E52L;
    case 53:   return 1E53L;
    case 54:   return 1E54L;
    case 55:   return 1E55L;
    case 56:   return 1E56L;
    case 57:   return 1E57L;
    case 58:   return 1E58L;
    case 59:   return 1E59L;
    case 60:   return 1E60L;
    case 61:   return 1E61L;
    case 62:   return 1E62L;
    case 63:   return 1E63L;
    case 64:   return 1E64L;
    case 65:   return 1E65L;
    case 66:   return 1E66L;
    case 67:   return 1E67L;
    case 68:   return 1E68L;
    case 69:   return 1E69L;
    case 70:   return 1E70L;
    case 71:   return 1E71L;
    case 72:   return 1E72L;
    case 73:   return 1E73L;
    case 74:   return 1E74L;
    case 75:   return 1E75L;
    case 76:   return 1E76L;
    case 77:   return 1E77L;
    case 78:   return 1E78L;
    case 79:   return 1E79L;
    case 80:   return 1E80L;
    case 81:   return 1E81L;
    case 82:   return 1E82L;
    case 83:   return 1E83L;
    case 84:   return 1E84L;
    case 85:   return 1E85L;
    case 86:   return 1E86L;
    case 87:   return 1E87L;
    case 88:   return 1E88L;
    case 89:   return 1E89L;
    case 90:   return 1E90L;
    case 91:   return 1E91L;
    case 92:   return 1E92L;
    case 93:   return 1E93L;
    case 94:   return 1E94L;
    case 95:   return 1E95L;
    case 96:   return 1E96L;
    case 97:   return 1E97L;
    case 98:   return 1E98L;
    case 99:   return 1E99L;
    case 100:  return 1E100L;
    case 101:  return 1E101L;
    case 102:  return 1E102L;
    case 103:  return 1E103L;
    case 104:  return 1E104L;
    case 105:  return 1E105L;
    case 106:  return 1E106L;
    case 107:  return 1E107L;
    case 108:  return 1E108L;
    case 109:  return 1E109L;
    case 110:  return 1E110L;
    case 111:  return 1E111L;
    case 112:  return 1E112L;
    case 113:  return 1E113L;
    case 114:  return 1E114L;
    case 115:  return 1E115L;
    case 116:  return 1E116L;
    case 117:  return 1E117L;
    case 118:  return 1E118L;
    case 119:  return 1E119L;
    case 120:  return 1E120L;
    case 121:  return 1E121L;
    case 122:  return 1E122L;
    case 123:  return 1E123L;
    case 124:  return 1E124L;
    case 125:  return 1E125L;
    case 126:  return 1E126L;
    case 127:  return 1E127L;
    case 128:  return 1E128L;
    case 129:  return 1E129L;
    case 130:  return 1E130L;
    case 131:  return 1E131L;
    case 132:  return 1E132L;
    case 133:  return 1E133L;
    case 134:  return 1E134L;
    case 135:  return 1E135L;
    case 136:  return 1E136L;
    case 137:  return 1E137L;
    case 138:  return 1E138L;
    case 139:  return 1E139L;
    case 140:  return 1E140L;
    case 141:  return 1E141L;
    case 142:  return 1E142L;
    case 143:  return 1E143L;
    case 144:  return 1E144L;
    case 145:  return 1E145L;
    case 146:  return 1E146L;
    case 147:  return 1E147L;
    case 148:  return 1E148L;
    case 149:  return 1E149L;
    case 150:  return 1E150L;
    case 151:  return 1E151L;
    case 152:  return 1E152L;
    case 153:  return 1E153L;
    case 154:  return 1E154L;
    case 155:  return 1E155L;
    case 156:  return 1E156L;
    case 157:  return 1E157L;
    case 158:  return 1E158L;
    case 159:  return 1E159L;
    case 160:  return 1E160L;
    case 161:  return 1E161L;
    case 162:  return 1E162L;
    case 163:  return 1E163L;
    case 164:  return 1E164L;
    case 165:  return 1E165L;
    case 166:  return 1E166L;
    case 167:  return 1E167L;
    case 168:  return 1E168L;
    case 169:  return 1E169L;
    case 170:  return 1E170L;
    case 171:  return 1E171L;
    case 172:  return 1E172L;
    case 173:  return 1E173L;
    case 174:  return 1E174L;
    case 175:  return 1E175L;
    case 176:  return 1E176L;
    case 177:  return 1E177L;
    case 178:  return 1E178L;
    case 179:  return 1E179L;
    case 180:  return 1E180L;
    case 181:  return 1E181L;
    case 182:  return 1E182L;
    case 183:  return 1E183L;
    case 184:  return 1E184L;
    case 185:  return 1E185L;
    case 186:  return 1E186L;
    case 187:  return 1E187L;
    case 188:  return 1E188L;
    case 189:  return 1E189L;
    case 190:  return 1E190L;
    case 191:  return 1E191L;
    case 192:  return 1E192L;
    case 193:  return 1E193L;
    case 194:  return 1E194L;
    case 195:  return 1E195L;
    case 196:  return 1E196L;
    case 197:  return 1E197L;
    case 198:  return 1E198L;
    case 199:  return 1E199L;
    case 200:  return 1E200L;
    case 201:  return 1E201L;
    case 202:  return 1E202L;
    case 203:  return 1E203L;
    case 204:  return 1E204L;
    case 205:  return 1E205L;
    case 206:  return 1E206L;
    case 207:  return 1E207L;
    case 208:  return 1E208L;
    case 209:  return 1E209L;
    case 210:  return 1E210L;
    case 211:  return 1E211L;
    case 212:  return 1E212L;
    case 213:  return 1E213L;
    case 214:  return 1E214L;
    case 215:  return 1E215L;
    case 216:  return 1E216L;
    case 217:  return 1E217L;
    case 218:  return 1E218L;
    case 219:  return 1E219L;
    case 220:  return 1E220L;
    case 221:  return 1E221L;
    case 222:  return 1E222L;
    case 223:  return 1E223L;
    case 224:  return 1E224L;
    case 225:  return 1E225L;
    case 226:  return 1E226L;
    case 227:  return 1E227L;
    case 228:  return 1E228L;
    case 229:  return 1E229L;
    case 230:  return 1E230L;
    case 231:  return 1E231L;
    case 232:  return 1E232L;
    case 233:  return 1E233L;
    case 234:  return 1E234L;
    case 235:  return 1E235L;
    case 236:  return 1E236L;
    case 237:  return 1E237L;
    case 238:  return 1E238L;
    case 239:  return 1E239L;
    case 240:  return 1E240L;
    case 241:  return 1E241L;
    case 242:  return 1E242L;
    case 243:  return 1E243L;
    case 244:  return 1E244L;
    case 245:  return 1E245L;
    case 246:  return 1E246L;
    case 247:  return 1E247L;
    case 248:  return 1E248L;
    case 249:  return 1E249L;
    case 250:  return 1E250L;
    case 251:  return 1E251L;
    case 252:  return 1E252L;
    case 253:  return 1E253L;
    case 254:  return 1E254L;
    case 255:  return 1E255L;
    case 256:  return 1E256L;
    case 257:  return 1E257L;
    case 258:  return 1E258L;
    case 259:  return 1E259L;
    case 260:  return 1E260L;
    case 261:  return 1E261L;
    case 262:  return 1E262L;
    case 263:  return 1E263L;
    case 264:  return 1E264L;
    case 265:  return 1E265L;
    case 266:  return 1E266L;
    case 267:  return 1E267L;
    case 268:  return 1E268L;
    case 269:  return 1E269L;
    case 270:  return 1E270L;
    case 271:  return 1E271L;
    case 272:  return 1E272L;
    case 273:  return 1E273L;
    case 274:  return 1E274L;
    case 275:  return 1E275L;
    case 276:  return 1E276L;
    case 277:  return 1E277L;
    case 278:  return 1E278L;
    case 279:  return 1E279L;
    case 280:  return 1E280L;
    case 281:  return 1E281L;
    case 282:  return 1E282L;
    case 283:  return 1E283L;
    case 284:  return 1E284L;
    case 285:  return 1E285L;
    case 286:  return 1E286L;
    case 287:  return 1E287L;
    case 288:  return 1E288L;
    case 289:  return 1E289L;
    case 290:  return 1E290L;
    case 291:  return 1E291L;
    case 292:  return 1E292L;
    case 293:  return 1E293L;
    case 294:  return 1E294L;
    case 295:  return 1E295L;
    case 296:  return 1E296L;
    case 297:  return 1E297L;
    case 298:  return 1E298L;
    case 299:  return 1E299L;
    case 300:  return 1E300L;
    case 301:  return 1E301L;
    case 302:  return 1E302L;
    case 303:  return 1E303L;
    case 304:  return 1E304L;
    case 305:  return 1E305L;
    case 306:  return 1E306L;
    case 307:  return 1E307L;
    case 308:  return 1E308L;
    case -1:   return 1E-1L;
    case -2:   return 1E-2L;
    case -3:   return 1E-3L;
    case -4:   return 1E-4L;
    case -5:   return 1E-5L;
    case -6:   return 1E-6L;
    case -7:   return 1E-7L;
    case -8:   return 1E-8L;
    case -9:   return 1E-9L;
    case -10:  return 1E-10L;
    case -11:  return 1E-11L;
    case -12:  return 1E-12L;
    case -13:  return 1E-13L;
    case -14:  return 1E-14L;
    case -15:  return 1E-15L;
    case -16:  return 1E-16L;
    case -17:  return 1E-17L;
    case -18:  return 1E-18L;
    case -19:  return 1E-19L;
    case -20:  return 1E-20L;
    case -21:  return 1E-21L;
    case -22:  return 1E-22L;
    case -23:  return 1E-23L;
    case -24:  return 1E-24L;
    case -25:  return 1E-25L;
    case -26:  return 1E-26L;
    case -27:  return 1E-27L;
    case -28:  return 1E-28L;
    case -29:  return 1E-29L;
    case -30:  return 1E-30L;
    case -31:  return 1E-31L;
    case -32:  return 1E-32L;
    case -33:  return 1E-33L;
    case -34:  return 1E-34L;
    case -35:  return 1E-35L;
    case -36:  return 1E-36L;
    case -37:  return 1E-37L;
    case -38:  return 1E-38L;
    case -39:  return 1E-39L;
    case -40:  return 1E-40L;
    case -41:  return 1E-41L;
    case -42:  return 1E-42L;
    case -43:  return 1E-43L;
    case -44:  return 1E-44L;
    case -45:  return 1E-45L;
    case -46:  return 1E-46L;
    case -47:  return 1E-47L;
    case -48:  return 1E-48L;
    case -49:  return 1E-49L;
    case -50:  return 1E-50L;
    case -51:  return 1E-51L;
    case -52:  return 1E-52L;
    case -53:  return 1E-53L;
    case -54:  return 1E-54L;
    case -55:  return 1E-55L;
    case -56:  return 1E-56L;
    case -57:  return 1E-57L;
    case -58:  return 1E-58L;
    case -59:  return 1E-59L;
    case -60:  return 1E-60L;
    case -61:  return 1E-61L;
    case -62:  return 1E-62L;
    case -63:  return 1E-63L;
    case -64:  return 1E-64L;
    case -65:  return 1E-65L;
    case -66:  return 1E-66L;
    case -67:  return 1E-67L;
    case -68:  return 1E-68L;
    case -69:  return 1E-69L;
    case -70:  return 1E-70L;
    case -71:  return 1E-71L;
    case -72:  return 1E-72L;
    case -73:  return 1E-73L;
    case -74:  return 1E-74L;
    case -75:  return 1E-75L;
    case -76:  return 1E-76L;
    case -77:  return 1E-77L;
    case -78:  return 1E-78L;
    case -79:  return 1E-79L;
    case -80:  return 1E-80L;
    case -81:  return 1E-81L;
    case -82:  return 1E-82L;
    case -83:  return 1E-83L;
    case -84:  return 1E-84L;
    case -85:  return 1E-85L;
    case -86:  return 1E-86L;
    case -87:  return 1E-87L;
    case -88:  return 1E-88L;
    case -89:  return 1E-89L;
    case -90:  return 1E-90L;
    case -91:  return 1E-91L;
    case -92:  return 1E-92L;
    case -93:  return 1E-93L;
    case -94:  return 1E-94L;
    case -95:  return 1E-95L;
    case -96:  return 1E-96L;
    case -97:  return 1E-97L;
    case -98:  return 1E-98L;
    case -99:  return 1E-99L;
    case -100: return 1E-100L;
    case -101: return 1E-101L;
    case -102: return 1E-102L;
    case -103: return 1E-103L;
    case -104: return 1E-104L;
    case -105: return 1E-105L;
    case -106: return 1E-106L;
    case -107: return 1E-107L;
    case -108: return 1E-108L;
    case -109: return 1E-109L;
    case -110: return 1E-110L;
    case -111: return 1E-111L;
    case -112: return 1E-112L;
    case -113: return 1E-113L;
    case -114: return 1E-114L;
    case -115: return 1E-115L;
    case -116: return 1E-116L;
    case -117: return 1E-117L;
    case -118: return 1E-118L;
    case -119: return 1E-119L;
    case -120: return 1E-120L;
    case -121: return 1E-121L;
    case -122: return 1E-122L;
    case -123: return 1E-123L;
    case -124: return 1E-124L;
    case -125: return 1E-125L;
    case -126: return 1E-126L;
    case -127: return 1E-127L;
    case -128: return 1E-128L;
    case -129: return 1E-129L;
    case -130: return 1E-130L;
    case -131: return 1E-131L;
    case -132: return 1E-132L;
    case -133: return 1E-133L;
    case -134: return 1E-134L;
    case -135: return 1E-135L;
    case -136: return 1E-136L;
    case -137: return 1E-137L;
    case -138: return 1E-138L;
    case -139: return 1E-139L;
    case -140: return 1E-140L;
    case -141: return 1E-141L;
    case -142: return 1E-142L;
    case -143: return 1E-143L;
    case -144: return 1E-144L;
    case -145: return 1E-145L;
    case -146: return 1E-146L;
    case -147: return 1E-147L;
    case -148: return 1E-148L;
    case -149: return 1E-149L;
    case -150: return 1E-150L;
    case -151: return 1E-151L;
    case -152: return 1E-152L;
    case -153: return 1E-153L;
    case -154: return 1E-154L;
    case -155: return 1E-155L;
    case -156: return 1E-156L;
    case -157: return 1E-157L;
    case -158: return 1E-158L;
    case -159: return 1E-159L;
    case -160: return 1E-160L;
    case -161: return 1E-161L;
    case -162: return 1E-162L;
    case -163: return 1E-163L;
    case -164: return 1E-164L;
    case -165: return 1E-165L;
    case -166: return 1E-166L;
    case -167: return 1E-167L;
    case -168: return 1E-168L;
    case -169: return 1E-169L;
    case -170: return 1E-170L;
    case -171: return 1E-171L;
    case -172: return 1E-172L;
    case -173: return 1E-173L;
    case -174: return 1E-174L;
    case -175: return 1E-175L;
    case -176: return 1E-176L;
    case -177: return 1E-177L;
    case -178: return 1E-178L;
    case -179: return 1E-179L;
    case -180: return 1E-180L;
    case -181: return 1E-181L;
    case -182: return 1E-182L;
    case -183: return 1E-183L;
    case -184: return 1E-184L;
    case -185: return 1E-185L;
    case -186: return 1E-186L;
    case -187: return 1E-187L;
    case -188: return 1E-188L;
    case -189: return 1E-189L;
    case -190: return 1E-190L;
    case -191: return 1E-191L;
    case -192: return 1E-192L;
    case -193: return 1E-193L;
    case -194: return 1E-194L;
    case -195: return 1E-195L;
    case -196: return 1E-196L;
    case -197: return 1E-197L;
    case -198: return 1E-198L;
    case -199: return 1E-199L;
    case -200: return 1E-200L;
    case -201: return 1E-201L;
    case -202: return 1E-202L;
    case -203: return 1E-203L;
    case -204: return 1E-204L;
    case -205: return 1E-205L;
    case -206: return 1E-206L;
    case -207: return 1E-207L;
    case -208: return 1E-208L;
    case -209: return 1E-209L;
    case -210: return 1E-210L;
    case -211: return 1E-211L;
    case -212: return 1E-212L;
    case -213: return 1E-213L;
    case -214: return 1E-214L;
    case -215: return 1E-215L;
    case -216: return 1E-216L;
    case -217: return 1E-217L;
    case -218: return 1E-218L;
    case -219: return 1E-219L;
    case -220: return 1E-220L;
    case -221: return 1E-221L;
    case -222: return 1E-222L;
    case -223: return 1E-223L;
    case -224: return 1E-224L;
    case -225: return 1E-225L;
    case -226: return 1E-226L;
    case -227: return 1E-227L;
    case -228: return 1E-228L;
    case -229: return 1E-229L;
    case -230: return 1E-230L;
    case -231: return 1E-231L;
    case -232: return 1E-232L;
    case -233: return 1E-233L;
    case -234: return 1E-234L;
    case -235: return 1E-235L;
    case -236: return 1E-236L;
    case -237: return 1E-237L;
    case -238: return 1E-238L;
    case -239: return 1E-239L;
    case -240: return 1E-240L;
    case -241: return 1E-241L;
    case -242: return 1E-242L;
    case -243: return 1E-243L;
    case -244: return 1E-244L;
    case -245: return 1E-245L;
    case -246: return 1E-246L;
    case -247: return 1E-247L;
    case -248: return 1E-248L;
    case -249: return 1E-249L;
    case -250: return 1E-250L;
    case -251: return 1E-251L;
    case -252: return 1E-252L;
    case -253: return 1E-253L;
    case -254: return 1E-254L;
    case -255: return 1E-255L;
    case -256: return 1E-256L;
    case -257: return 1E-257L;
    case -258: return 1E-258L;
    case -259: return 1E-259L;
    case -260: return 1E-260L;
    case -261: return 1E-261L;
    case -262: return 1E-262L;
    case -263: return 1E-263L;
    case -264: return 1E-264L;
    case -265: return 1E-265L;
    case -266: return 1E-266L;
    case -267: return 1E-267L;
    case -268: return 1E-268L;
    case -269: return 1E-269L;
    case -270: return 1E-270L;
    case -271: return 1E-271L;
    case -272: return 1E-272L;
    case -273: return 1E-273L;
    case -274: return 1E-274L;
    case -275: return 1E-275L;
    case -276: return 1E-276L;
    case -277: return 1E-277L;
    case -278: return 1E-278L;
    case -279: return 1E-279L;
    case -280: return 1E-280L;
    case -281: return 1E-281L;
    case -282: return 1E-282L;
    case -283: return 1E-283L;
    case -284: return 1E-284L;
    case -285: return 1E-285L;
    case -286: return 1E-286L;
    case -287: return 1E-287L;
    case -288: return 1E-288L;
    case -289: return 1E-289L;
    case -290: return 1E-290L;
    case -291: return 1E-291L;
    case -292: return 1E-292L;
    case -293: return 1E-293L;
    case -294: return 1E-294L;
    case -295: return 1E-295L;
    case -296: return 1E-296L;
    case -297: return 1E-297L;
    case -298: return 1E-298L;
    case -299: return 1E-299L;
    case -300: return 1E-300L;
    case -301: return 1E-301L;
    case -302: return 1E-302L;
    case -303: return 1E-303L;
    case -304: return 1E-304L;
    case -305: return 1E-305L;
    case -306: return 1E-306L;
    case -307: return 1E-307L;
    case -308: return 1E-308L;
    default: return expon < 0 ? 1E-308L : 1E308L;
    }
}
