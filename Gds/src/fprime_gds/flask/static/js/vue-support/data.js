
Vue.component("b-data", {
    template: "#data-template",
    props: ['current'],
    data: () => ({
        pressures:[],
        labels:[],
        chart:""
    }),
    methods:{
        async getData(update){
          let data = await (await fetch('http://127.0.0.1:5000/pressures')).json();
          if(!update){
            if(data[0] != -1){
                for(let i=0; i<data.length; ++i){
                  this.pressures.push(data[i]["y"]);
                  this.labels.push(data[i]["x"]);
                }
            }
              
          }else{
            this.pressures.push(data[data.length-1]["y"])
            this.pressures.push(data[data.length-1]["x"])
            this.chart.update();
          }
        },
        async createGraph(){
            await this.getData(false);
            let ctx = this.$refs['my-canvas'].getContext('2d')
            this.chart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels:this.labels,
                    datasets: [{
                        label:"pressures",
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
        }
    },
    watch:{
      current(val){
        this.pressures = [];
        if (val == "Data"){
            this.getData(true);
        }
      }
    },
    mounted(){
        this.createGraph();
    },
   
  });