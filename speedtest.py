#test de velocidad cortecia de la ia
import requests
import time
import statistics

def medir_tiempo_descarga_imagenes():
    # Lista de URLs proporcionadas
    urls = [
        "https://img1tmo.com/uploads/20160411/5af726382f142/001_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/002_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/003_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/004_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/005_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/006_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/007_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/008_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/009_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/10_570af7ab0b9ac.webp",
        "https://img1tmo.com/uploads/20160411/5af726382f142/11_570af7ab0b9ac.webp"
    ]
    
    # Headers para "engañar" al servidor
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
    
    print("🔍 Iniciando medición de tiempo de descarga de imágenes...")
    print(f"📊 Total de imágenes a medir: {len(urls)}")
    print("🔐 Usando headers especiales para el servidor...")
    print("⏳ Esto puede tomar unos segundos...\n")
    
    tiempos = []
    tamanos = []
    exitosas = 0
    
    for i, url in enumerate(urls, 1):
        try:
            print(f"📥 Descargando imagen {i}/{len(urls)}...", end=" ")
            inicio = time.time()
            
            # Usar los headers especiales en la solicitud
            response = requests.get(url, headers=headers, timeout=30)
            
            if response.status_code == 200:
                fin = time.time()
                tiempo_descarga = fin - inicio
                tamano_bytes = len(response.content)
                tamanos.append(tamano_bytes)
                tiempos.append(tiempo_descarga)
                exitosas += 1
                
                print(f"✅ {tiempo_descarga:.3f}s | {tamano_bytes/1024:.1f} KB")
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
        # Estadísticas de tiempo
        tiempo_promedio = statistics.mean(tiempos)
        tiempo_min = min(tiempos)
        tiempo_max = max(tiempos)
        desviacion_std = statistics.stdev(tiempos) if len(tiempos) > 1 else 0
        
        # Estadísticas de tamaño
        tamano_promedio = statistics.mean(tamanos)
        tamano_total = sum(tamanos)
        
        print(f"✅ Descargas exitosas: {exitosas}/{len(urls)}")
        print(f"⏱️  Tiempo promedio por imagen: {tiempo_promedio:.3f} segundos")
        print(f"⚡ Tiempo más rápido: {tiempo_min:.3f} segundos")
        print(f"🐌 Tiempo más lento: {tiempo_max:.3f} segundos")
        print(f"📈 Desviación estándar: {desviacion_std:.3f} segundos")
        print(f"💾 Tamaño promedio: {tamano_promedio/1024:.1f} KB")
        print(f"📦 Tamaño total descargado: {tamano_total/1024:.1f} KB")
        
        if tiempo_promedio > 0:
            velocidad_mbps = (tamano_promedio * 8) / (tiempo_promedio * 1024 * 1024)
            print(f"🚀 Velocidad promedio: {velocidad_mbps:.2f} Mbps")
        
        # Tiempo total estimado para 10 imágenes
        tiempo_10_imagenes = tiempo_promedio * 10
        print(f"\n🎯 ESTIMACIÓN PARA 10 IMÁGENES:")
        print(f"⏱️  Tiempo promedio estimado: {tiempo_10_imagenes:.2f} segundos")
        
        # Mostrar los headers que se están usando
        print(f"\n🔧 Headers utilizados:")
        print(f"   Referer: {headers['Referer']}")
        print(f"   User-Agent: {headers['User-Agent'][:50]}...")
        
    else:
        print("❌ No se pudo descargar ninguna imagen")
        print("💡 Posibles soluciones:")
        print("   - Verifica tu conexión a internet")
        print("   - El servidor podría haber cambiado sus requisitos")
        print("   - Intenta con diferentes headers")

if __name__ == "__main__":
    # Instalar requests si no está instalado
    try:
        import requests
    except ImportError:
        print("❌ La librería 'requests' no está instalada.")
        print("💡 Instálala con: pip install requests")
        exit(1)
    
    medir_tiempo_descarga_imagenes()
