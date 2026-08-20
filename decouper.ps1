# Decoupe une video en tranches de 45s et les convertit en vertical 9:16 pour TikTok
#
# Usage : .\decoupe_tiktok.ps1 gameplay.mp4
#
# Sorties :
#   - clips\clip_001.mp4, clip_002.mp4, ... (decoupe brute, sans re-encodage)
#   - clips_vertical\clip_001.mp4, ... (format 1080x1920 pret pour TikTok)

param(
    [Parameter(Mandatory=$true)]
    [string]$Input
)

$Duree = 45   # duree de chaque tranche en secondes

if (-not (Test-Path $Input)) {
    Write-Host "Erreur : fichier '$Input' introuvable"
    exit 1
}

New-Item -ItemType Directory -Force -Path "clips" | Out-Null
New-Item -ItemType Directory -Force -Path "clips_vertical" | Out-Null

# Recupere la duree totale de la video en secondes
$DureeTotaleStr = ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 $Input
$DureeTotale = [math]::Floor([double]$DureeTotaleStr)

Write-Host "Duree totale : ${DureeTotale}s -> decoupage en tranches de ${Duree}s"

$Num = 1
$Debut = 0

while ($Debut -lt $DureeTotale) {
    $Nom = "clip_{0:D3}.mp4" -f $Num

    Write-Host "-> Decoupe $Nom (debut: ${Debut}s)"

    # Decoupe brute, sans re-encodage (rapide, coupe au keyframe le plus proche)
    ffmpeg -y -loglevel error -i $Input -ss $Debut -t $Duree -c copy "clips\$Nom"

    # Conversion en vertical 9:16, crop centre (ajuste le parametre crop si l'action
    # n'est pas au centre de l'ecran, ex: crop=ih*9/16:ih:x=iw-ih*9/16 pour prendre la droite)
    Write-Host "   Conversion en vertical..."
    ffmpeg -y -loglevel error -i "clips\$Nom" `
        -vf "crop=ih*9/16:ih,scale=1080:1920" `
        -c:v libx264 -crf 18 -preset fast -c:a aac `
        "clips_vertical\$Nom"

    $Debut += $Duree
    $Num += 1
}

Write-Host ""
Write-Host "Termine : $($Num - 1) clips crees."
Write-Host "  - Bruts     : clips\"
Write-Host "  - Verticaux : clips_vertical\  (a uploader sur TikTok)"