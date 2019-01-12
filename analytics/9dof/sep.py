
with open('log0.txt', 'r') as log:
    fGforce = open('log_raw_g.txt', 'w')
    fRoll   = open('log_roll.txt', 'w')
    fMag    = open('log_mag.txt', 'w')

    for line in log:
        fields = line.split(', ')

        stringGforce = fields[0] + ',' + fields[1] + ',' + fields[2] + ',' + fields[3] + '\n'
        stringRoll = fields[0] + ',' + fields[4] + ',' + fields[5] + ',' + fields[6] + '\n'
        stringMag = fields[0] + ',' + fields[7] + ',' + fields[8] + ',' + fields[9] + '\n'

        fGforce.write(stringGforce)

        fRoll.write(stringRoll)

        fMag.write(stringMag)

fGforce.close
fRoll.close
fMag.close
