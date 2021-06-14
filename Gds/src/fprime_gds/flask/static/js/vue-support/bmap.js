Vue.component("b-map", {
    template: "#map-template",
    props: ['current'],
    data: () => ({
        latlngs:[],
        map:"",
        polyline:""
    }),
    methods:{
       async getGpsPos(remove){
        if(remove){
            this.map.removeLayer(this.polyline)
        }
        let data = await (await fetch('http://127.0.0.1:5000/gps')).json();
        if (data[0] != -1){
          for(let i= 0; i < data.length; i++){
           this.latlngs.push(new L.LatLng(data[i].y.latitude, data[i].y.longitude));
          } 
          this.polyline = L.polyline(this.latlngs, {color: 'red'})
          this.map.addLayer(this.polyline)
        }
       },
    },
    watch:{
      current(val){
        this.latlngs = [];
        if (val == "Map"){
          this.getGpsPos(true);
        }
      }
    },
    mounted() {
        this.map = L.map('map').setView([47.0024, 7.0126], 10);

        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        }).addTo(this.map);
        this.getGpsPos(false);
    }
  });