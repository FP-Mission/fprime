Vue.component("b-picture", {
  template: "#picture-template",
  props: ['current'],
  data: () => ({
      imgs:[],
      path:"/img/pictures/"
  }),
  methods:{
      async getImg(){
        let data = await (await fetch('http://127.0.0.1:5000/images')).json();
        if (data[0] != -1){
          for(let i= 0; i < data.length; i++){
            this.imgs.push(`${this.path}${data[i]}.jpeg`);
          } 
        }
      },
      caller(){
        if (this.current == "Picture"){
          this.imgs = [];
          this.getImg();
        }
      }
  },
  watch:{
    current(val){
      this.caller()
    }
  },
  mounted() {
    this.caller()
    setInterval(this.caller,10000)
  }
});