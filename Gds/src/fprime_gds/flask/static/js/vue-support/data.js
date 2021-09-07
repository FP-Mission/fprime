
Vue.component("b-data", {
    template: "#data-template",
    props: ['current'],
    data: () => ({
        pressures:[],
        labelsPressure:[],
        internalTemp:[],
        externalTemp:[],
        labelsTemp:[],
        chartPressure:"",
        chartAltitude:""
    }),
    methods:{
        async getData(){
          let dataPressure = await (await fetch('http://127.0.0.1:5000/pressures')).json();
          let internalTemp = await (await fetch('http://127.0.0.1:5000/internaltemp')).json();
          let externalTemp = await (await fetch('http://127.0.0.1:5000/externaltemp')).json();
            if(dataPressure[0] != -1){
                for(let i=0; i<dataPressure.length; ++i){
                  this.pressures.push(dataPressure[i]["y"]);
                  this.labelsPressure.push(dataPressure[i]["x"]);
                }
            }
            if(internalTemp[0] != -1){
              for(let i=0; i<internalTemp.length; ++i){
                this.internalTemp.push(internalTemp[i]["y"]);
                this.labelsTemp.push(internalTemp[i]["x"]);
                
              }
              
          }
          if(externalTemp[0] != -1){
            for(let i=0; i<externalTemp.length; ++i){
              this.externalTemp.push(externalTemp[i]["y"]);            
            }
            
        }

        },
        async createGraph(){
            await this.getData();
            let ctxPressure = this.$refs['chartPressure'].getContext('2d')
            let ctxTemp = this.$refs['chartTemp'].getContext('2d')
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
            this.chartAltitude = new Chart(ctxTemp, {
              type: 'line',
              data: {
                  labels:this.labelsAltitude,
                  datasets: [{
                      label:"Internal temperatures",
                      data: this.internalTemp,
                      tension: 0.4,
                      backgroundColor: [
                        'rgba(54, 162, 235, 0.2)',
                      ],
                      borderColor: [
                        'rgba(54, 162, 235, 1)',
                      ],
                      borderWidth: 1
                  },
                  {
                    label:"External temperatures",
                    data: this.externalTemp,
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