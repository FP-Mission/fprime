
Vue.component("b-data", {
    template: "#data-template",
    props: ['current'],
    data: () => ({
        pressures:[],
        labelsPressure:[],
        altitudes:[],
        labelsAltitude:[],
        chartPressure:"",
        chartAltitude:""
    }),
    methods:{
        async getData(){
          let dataPressure = await (await fetch('http://127.0.0.1:5000/pressures')).json();
          let dataAltitude = await (await fetch('http://127.0.0.1:5000/altitudes')).json();
            if(dataPressure[0] != -1){
                for(let i=0; i<dataPressure.length; ++i){
                  if(i%10 == 0){
                    this.pressures.push(dataPressure[i]["y"]);
                    this.labelsPressure.push(dataPressure[i]["x"]);
                  }
                }
            }
            if(dataAltitude[0] != -1){
              for(let i=0; i<dataAltitude.length; ++i){
                if(i%10 == 0){
                  this.altitudes.push(dataAltitude[i]["y"]);
                  this.labelsAltitude.push(dataAltitude[i]["x"]);
                }
              }
          }

        },
        async createGraph(){
            await this.getData();
            let ctxPressure = this.$refs['chartPressure'].getContext('2d')
            let ctxAltitude = this.$refs['chartAltitude'].getContext('2d')
            this.chartPressure = new Chart(ctxPressure, {
                type: 'line',
                data: {
                    labels:this.labelsPressure,
                    datasets: [{
                        label:"pressures",
                        tension: 0.4,
                        data: this.pressures,
                        backgroundColor: [
                            'rgba(255, 99, 132, 0.2)'
                        ],
                        borderColor: [
                            'rgba(255, 99, 132, 1)'
                        ],
                        borderWidth: 1
                    }]
                },
                 options: {
                      responsive: true,
    
                    },
                  
            });
            this.chartAltitude = new Chart(ctxAltitude, {
              type: 'line',
              data: {
                  labels:this.labelsAltitude,
                  datasets: [{
                      label:"altitudes",
                      data: this.altitudes,
                      tension: 0.4,
                      backgroundColor: [
                        'rgba(54, 162, 235, 0.2)',
                      ],
                      borderColor: [
                        'rgba(54, 162, 235, 1)',
                      ],
                      borderWidth: 1
                  }]
              },
               options: {
                    responsive: true,
                    scales: {
                      yAxes: [{
                        display: true,
                        ticks: {
                            beginAtZero: true,
                            steps: 100,
                            max: 40000
                        }
                    }]
                  }
                  },
                
          });
        }
    },
    watch:{
      current(val){
        if (val == "Data"){
          this.createGraph();
        }
      }
    },
    mounted(){
        this.createGraph();
    },
   
  });