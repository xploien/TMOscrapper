#test de velocidad cortecia de la ia
import requests
import time
import statistics
import os

def medir_tiempo_descarga_imagenes(carpeta_destino="imagenes_descargadas"):
    # Crear la carpeta de destino si no existe
    os.makedirs(carpeta_destino, exist_ok=True)
    print(f"📁 Carpeta de destino: {os.path.abspath(carpeta_destino)}")

    urls = [

                            "https://img1tmo.com/uploads/20210701/5387f9c16108d4a692c57a06ffdf6508/c51f6290.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/001_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/002_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/003_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/004_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/005_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/006_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/007_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/008_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/009_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/10_570af7ab0b9ac.webp",
        # "https://img1tmo.com/uploads/20160411/5af726382f142/11_570af7ab0b9ac.webp"
    ]

    headers = {
        'Referer': 'https://zonatmo.com/',
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
        'Accept': 'image/webp,image/apng,image/*,*/*;q=0.8',
        'Accept-Language': 'es-ES,es;q=0.9,en;q=0.8',
        'Accept-Encoding': 'gzip, deflate, br',
        'DNT': '1',
        'Connection': 'keep-alive',
        'Sec-Fetch-Dest': 'image',
        'Sec-Fetch-Mode': 'no-cors',
        'Sec-Fetch-Site': 'cross-site'
    }

    print(" Iniciando ")
    print(f"imagenes: {len(urls)}")

    tiempos = []
    tamanos = []
    exitosas = 0

    for i, url in enumerate(urls, 1):
        try:
            # Extraer el nombre del archivo desde la URL
            nombre_archivo = url.split("/")[-1]
            ruta_destino = os.path.join(carpeta_destino, nombre_archivo)

            print(f"Descargando imagen {i}/{len(urls)} ({nombre_archivo})...", end=" ")
            inicio = time.time()

            response = requests.get(url, headers=headers, timeout=30)

            if response.status_code == 200:
                fin = time.time()
                tiempo_descarga = fin - inicio
                tamano_bytes = len(response.content)
                tamanos.append(tamano_bytes)
                tiempos.append(tiempo_descarga)
                exitosas += 1

                # ✅ GUARDAR LA IMAGEN
                with open(ruta_destino, "wb") as f:
                    f.write(response.content)

                print(f"✅ {tiempo_descarga:.3f}s | {tamano_bytes/1024:.1f} KB | guardada en '{ruta_destino}'")
            else:
                print(f"❌ Error HTTP {response.status_code}")

        except requests.exceptions.Timeout:
            print("❌ Timeout")
        except requests.exceptions.ConnectionError:
            print("❌ Error de conexión")
        except Exception as e:
            print(f"❌ Error: {e}")

    print("\n" + "="*50)
    print("📊 RESULTADOS FINALES:")
    print("="*50)

    if exitosas > 0:
        tiempo_promedio = statistics.mean(tiempos)
        tiempo_min = min(tiempos)
        tiempo_max = max(tiempos)
        desviacion_std = statistics.stdev(tiempos) if len(tiempos) > 1 else 0
        tamano_promedio = statistics.mean(tamanos)
        tamano_total = sum(tamanos)

        print(f"✅ Descargas exitosas: {exitosas}/{len(urls)}")
        print(f"📁 Imágenes guardadas en: {os.path.abspath(carpeta_destino)}")
        print(f"⏱️  Tiempo promedio por imagen: {tiempo_promedio:.3f} segundos")
        print(f"⚡ Tiempo más rápido: {tiempo_min:.3f} segundos")
        print(f"🐌 Tiempo más lento: {tiempo_max:.3f} segundos")
        print(f"📈 Desviación estándar: {desviacion_std:.3f} segundos")
        print(f"💾 Tamaño promedio: {tamano_promedio/1024:.1f} KB")
        print(f"📦 Tamaño total descargado: {tamano_total/1024:.1f} KB")

        if tiempo_promedio > 0:
            velocidad_mbps = (tamano_promedio * 8) / (tiempo_promedio * 1024 * 1024)
            print(f"🚀 Velocidad promedio: {velocidad_mbps:.2f} Mbps")

        tiempo_10_imagenes = tiempo_promedio * 10
        print(f"\n🎯 ESTIMACIÓN PARA 10 IMÁGENES:")
        print(f"⏱️  Tiempo promedio estimado: {tiempo_10_imagenes:.2f} segundos")
    else:
        print("❌ No se pudo descargar ninguna imagen")

if __name__ == "__main__":
    try:
        import requests
    except ImportError:
        print("❌ La librería 'requests' no está instalada.")
        print("💡 Instálala con: pip install requests")
        exit(1)

    medir_tiempo_descarga_imagenes()
